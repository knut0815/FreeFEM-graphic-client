#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <future>
#include <glm/glm.hpp>
#include "Logger.h"
#include "Import.h"

namespace ffGraph {
namespace JSON {

static GeometryPrimitiveTopology GetMainPrimitiveTopology(std::string& j)
{
    if (j.compare("Curve2D") == 0 || j.compare("Curve3D") == 0)
        return GeometryPrimitiveTopology::GEO_PRIMITIVE_TOPOLOGY_LINE_LIST;
    else if (j.compare("Mesh2D") == 0 || j.compare("Mesh3D") == 0)
        return GeometryPrimitiveTopology::GEO_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    return GeometryPrimitiveTopology::GEO_PRIMITIVE_TOPOLOGY_COUNT;
}

static GeometryPrimitiveTopology GetBorderPrimitiveTopology(std::string& j)
{
    if (j.compare("Mesh2D") == 0)
        return GeometryPrimitiveTopology::GEO_PRIMITIVE_TOPOLOGY_LINE_LIST;
    else if (j.compare("Mesh3D") == 0)
        return GeometryPrimitiveTopology::GEO_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    return GeometryPrimitiveTopology::GEO_PRIMITIVE_TOPOLOGY_COUNT;
}

Geometry ConstructGeometry(std::vector<float> Vertices, std::vector<uint32_t> Indices, std::vector<int> Labels, LabelTable& Table)
{
    Geometry n;

    for (const auto& lab : Labels) {
        AddLabelToTable(Table, lab);
    }
    GenerateColorFromLabels(Table);

    n.Data = ffNewArray(Indices.size(), sizeof(Vertex));

    size_t size = Indices.size();
    Vertex *ptr = (Vertex *)n.Data.Data;
    for (size_t i = 0; i < size; ++i) {

        ptr[i].x = Vertices[Indices[i] * 3 + 0];
        ptr[i].y = Vertices[Indices[i] * 3 + 1];
        ptr[i].z = Vertices[Indices[i] * 3 + 2];
        ptr[i].r = 0.f;
        ptr[i].g = 0.f;
        ptr[i].b = 0.f;
        ptr[i].a = 1.f;
    }
    return n;
}

glm::vec2 IsoValue(glm::vec2 T[3], glm::vec2 BarycentricPoint)
{
    glm::mat2 mat(glm::vec2(T[1].x - T[0].x, T[1].y - T[0].y), glm::vec2(T[2].x - T[0].x, T[2].y - T[0].y));

    return mat * BarycentricPoint * T[0];
}

Geometry ConstructIsoMeshVector(std::vector<float>& Vertices, std::vector<uint32_t>& Indices, std::vector<float>& Values, float min, float max)
{
    Geometry n;

    n.Data = ffNewArray((Vertices.size() / 3LU) * 2LU, sizeof(Vertex));

    size_t size = Vertices.size() / 3LU;
    Vertex *ptr = (Vertex *)n.Data.Data;
    std::vector<float> tmp(Values.size());

    for (size_t i = 0; i < Values.size(); ++i) {
        std::cout << "Values[" << i << "] = " << Values[i] << "\n";
        tmp[i] = sqrt(Values[i * 2] * Values[i * 2] + Values[i * 2 + 1] * Values[i * 2 + 1]);
    }
    for (size_t i = 0; i < tmp.size(); ++i) {
        std::cout << tmp[i] << "\n";
        min = std::min(min, tmp[i]);
        max = std::max(max, tmp[i]);
    }
    std::cout << "MIN " << min << " MAX " << max << "\n";
    for (size_t i = 0; i < size; ++i) {

        ptr[i * 2].x = Vertices[i * 3 + 0];
        ptr[i * 2].y = Vertices[i * 3 + 1];
        ptr[i * 2].z = Vertices[i * 3 + 2];
        float H = 179.f * (tmp[i] - min) / (max - min);
        const Color& c = NewColor(H, 1.f, 1.f);
        ptr[i * 2].r = c.r;
        ptr[i * 2].g = c.g;
        ptr[i * 2].b = c.b;
        ptr[i * 2].a = c.a;

        ptr[i * 2 + 1].x = Vertices[i * 3 + 0] + Values[i * 2] / max;
        ptr[i * 2 + 1].y = Vertices[i * 3 + 1] + Values[i * 2 + 1] / max;
        ptr[i * 2 + 1].z = Vertices[i * 3 + 2];

        ptr[i * 2 + 1].r = c.r;
        ptr[i * 2 + 1].g = c.g;
        ptr[i * 2 + 1].b = c.b;
        ptr[i * 2 + 1].a = c.a;
    }
    return n;
}

Geometry ConstructIsoMesh(std::vector<float>& Vertices, std::vector<uint32_t>& Indices, std::vector<float>& Values, float min, float max)
{
    Geometry n;

    n.Data = ffNewArray(Indices.size(), sizeof(Vertex));

    size_t size = Indices.size();
    Vertex *ptr = (Vertex *)n.Data.Data;
    std::vector<float> tmp(Indices.size());

    for (size_t i = 0; i < tmp.size(); ++i) {
        tmp[i] = Values[Indices[i]];
    }
    for (size_t i = 0; i < tmp.size(); ++i) {
        min = std::min(min, tmp[i]);
        max = std::max(max, tmp[i]);
    }
    for (size_t i = 0; i < size; ++i) {

        ptr[i].x = Vertices[Indices[i] * 3 + 0];
        ptr[i].y = Vertices[Indices[i] * 3 + 1];
        ptr[i].z = Vertices[Indices[i] * 3 + 2];
        float H = 330.f * (tmp[i] - min) / (max - min);
        const Color& c = NewColor(H, 1.f, 1.f);
        ptr[i].r = c.r;
        ptr[i].g = c.g;
        ptr[i].b = c.b;
        ptr[i].a = c.a;
    }
    return n;
}

void ImportGeometry(json GeoJSON, ThreadSafeQueue *Queue, uint16_t PlotID)
{
    LabelTable Table;

    std::string GeoType = GeoJSON["Type"].get<std::string>();
    uint16_t MeshID = GeoJSON["Id"].get<uint16_t>();
    ConstructedGeometry Data(PlotID, MeshID);

    std::vector<float> Vertices = GeoJSON["Vertices"].get<std::vector<float>>();
    std::vector<uint32_t> Indices = GeoJSON["MeshIndices"].get<std::vector<uint32_t>>();
    std::vector<int> Labels = GeoJSON["MeshLabels"].get<std::vector<int>>();

    Data.Geo = ConstructGeometry(Vertices, Indices, Labels, Table);
    if (Data.Geo.Data.Data == 0) {
        LogWarning("AsyncImport", "Failed to import mesh.");
    } else {
        Data.Geo.Description.PrimitiveTopology = GetMainPrimitiveTopology(GeoType);
        Data.Geo.Description.PolygonMode = GEO_POLYGON_MODE_LINE;
        Queue->push(Data);
    }

    bool AsIsoValues = GeoJSON["IsoValues"].get<bool>();
    if (AsIsoValues) {
        std::cout << "Importing isovalues.\n";
        ConstructedGeometry IsoValues(PlotID, MeshID);

        std::vector<float> values = GeoJSON["IsoV1"].get<std::vector<float>>();
        std::vector<float> referencetriangle = GeoJSON["IsoPSub"].get<std::vector<float>>();
        bool IsoAsVector = GeoJSON["IsoVector"].get<bool>();
        if (IsoAsVector) {
            IsoValues.Geo = ConstructIsoMeshVector(Vertices, Indices, values, GeoJSON["IsoMin"].get<float>(), GeoJSON["IsoMax"].get<float>());
            IsoValues.Geo.Description.PrimitiveTopology = GeometryPrimitiveTopology::GEO_PRIMITIVE_TOPOLOGY_LINE_LIST;
        } else {
            IsoValues.Geo = ConstructIsoMesh(Vertices, Indices, values, GeoJSON["IsoMin"].get<float>(), GeoJSON["IsoMax"].get<float>());
            IsoValues.Geo.Description.PrimitiveTopology = GeometryPrimitiveTopology::GEO_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        }
        IsoValues.Geo.Description.PolygonMode = GEO_POLYGON_MODE_LINE;
        Queue->push(IsoValues);
    }

    bool AsBorder = GeoJSON["Borders"].get<bool>();
    if (AsBorder) {
        std::cout << "Import border.\n";
        ConstructedGeometry Border(PlotID, MeshID);

        Indices.clear();
        Labels.clear();

        std::vector<uint32_t> Indices = GeoJSON["BorderIndices"].get<std::vector<uint32_t>>();
        std::vector<int> Labels = GeoJSON["BorderLabels"].get<std::vector<int>>();

        Border.Geo = ConstructGeometry(Vertices, Indices, Labels, Table);

        if (Border.Geo.Data.Data == 0) {
            LogWarning("AsyncImport", "Failed to import border.");
        } else {
            Border.Geo.Description.PrimitiveTopology = GetBorderPrimitiveTopology(GeoType);
            Border.Geo.Description.PolygonMode = GEO_POLYGON_MODE_LINE;
            //Queue->push(Border);
        }
    }
    std::cout << "Finished importing data.\n";
}

void AsyncImport(std::string CompressedJSON, ThreadSafeQueue& Queue)
{
    json j = json::from_cbor(CompressedJSON);

    uint16_t PlotID = j["Plot"].get<uint16_t>();

    for (auto & Geometry : j["Geometry"]) {
        //ImportGeometry(Geometry, &Queue, PlotID);
        std::async(std::launch::async, ImportGeometry, Geometry, &Queue, PlotID);
    }
}

}    // namespace JSON
}    // namespace ffGraph