#include "GraphConstructor.h"
#include "Logger.h"

namespace ffGraph {
namespace Vulkan {

static RenderGraphNode FillRenderGraphNode(const VmaAllocator& Allocator, JSON::SceneObject& Obj) {
    RenderGraphNode Node;

    Node.GeoType = Obj.GeoType;
    Node.BatchDimension = Obj.DataType;
    Node.to_render = (Obj.GeoType == JSON::GeometryType::Volume) ? false : true;
    Node.CPUMeshData = newBatch(Obj);
    Node.LineWidth = Obj.LineWidth;

    return Node;
}

RenderGraphNode FillRenderGraphNode(const VmaAllocator& Allocator, Array& Data, JSON::GeometryType GeoType, JSON::Dimension n, int LineWidth) {
    RenderGraphNode Node;

    Node.Layout = VK_NULL_HANDLE;
    Node.Handle = VK_NULL_HANDLE;

    Node.GeoType = GeoType;
    Node.BatchDimension = n;
    Node.to_render = (Node.GeoType == JSON::GeometryType::Volume) ? false : true;
    Node.CPUMeshData = newBatch({Data}, (n == JSON::Dimension::Mesh2D) ? VK_PRIMITIVE_TOPOLOGY_LINE_LIST : VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    Node.LineWidth = LineWidth;

    return Node;
}

static RenderGraphNode ConstructRenderGraphNode(const Device& D, const VkRenderPass& Renderpass, const Resource& r, RenderGraphNode& Node) {

    VkPushConstantRange PushConstantRange = {};
    PushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    PushConstantRange.offset = 0;
    PushConstantRange.size = sizeof(CameraUniform);

    // Building VkPipeline
    VkPipelineLayoutCreateInfo PipelineLayoutInfo = {};
    PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    PipelineLayoutInfo.pushConstantRangeCount = 1;
    PipelineLayoutInfo.pPushConstantRanges = &PushConstantRange;

    vkCreatePipelineLayout(D.Handle, &PipelineLayoutInfo, 0, &Node.Layout);

    VkPipelineShaderStageCreateInfo ShaderStageInfo[2] = {};
    if (Node.BatchDimension == JSON::Dimension::Mesh2D) {
        ShaderStageInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ShaderStageInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        ShaderStageInfo[0].module = r.Shaders[0].Module;
        ShaderStageInfo[0].pName = "main";
    } else {
        ShaderStageInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ShaderStageInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        ShaderStageInfo[0].module = r.Shaders[1].Module;
        ShaderStageInfo[0].pName = "main";
    }

    ShaderStageInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    ShaderStageInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    ShaderStageInfo[1].module = r.Shaders[2].Module;
    ShaderStageInfo[1].pName = "main";

    int BindingInput = 0;
    std::vector<VkVertexInputBindingDescription> vertexInputBindingDescription = {};

    VkVertexInputBindingDescription inputBindingDescription;
    VkVertexInputAttributeDescription inputAttributeDescription[2] = {};
    if (Node.BatchDimension == JSON::Dimension::Mesh2D) {
        inputBindingDescription.binding = BindingInput;
        inputBindingDescription.stride = sizeof(float) * 6;
        inputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        vertexInputBindingDescription.push_back(inputBindingDescription);

        inputAttributeDescription[0].binding = 0;
        inputAttributeDescription[0].location = 0;
        inputAttributeDescription[0].format = VK_FORMAT_R32G32_SFLOAT;
        inputAttributeDescription[0].offset = sizeof(float) * 0;

        inputAttributeDescription[1].binding = 0;
        inputAttributeDescription[1].location = 1;
        inputAttributeDescription[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        inputAttributeDescription[1].offset = sizeof(float) * 2;

    } else {
        inputBindingDescription.binding = BindingInput;
        inputBindingDescription.stride = sizeof(float) * 7;
        inputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        vertexInputBindingDescription.push_back(inputBindingDescription);

        inputAttributeDescription[0].binding = 0;
        inputAttributeDescription[0].location = 0;
        inputAttributeDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        inputAttributeDescription[0].offset = sizeof(float) * 0;

        inputAttributeDescription[1].binding = 0;
        inputAttributeDescription[1].location = 1;
        inputAttributeDescription[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        inputAttributeDescription[1].offset = sizeof(float) * 3;
    }

    VkPipelineVertexInputStateCreateInfo VertexInputStateInfo = {};
    VertexInputStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    VertexInputStateInfo.vertexBindingDescriptionCount = (uint32_t)vertexInputBindingDescription.size( );
    VertexInputStateInfo.pVertexBindingDescriptions = vertexInputBindingDescription.data( );
    VertexInputStateInfo.vertexAttributeDescriptionCount = (uint32_t)2;
    VertexInputStateInfo.pVertexAttributeDescriptions = inputAttributeDescription;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {};
    inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateCreateInfo.topology = Node.CPUMeshData.Topology;
    inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

    VkDynamicState dynamicStateEnables[2] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.dynamicStateCount = 2;
    dynamicStateCreateInfo.pDynamicStates = dynamicStateEnables;

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
    rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCreateInfo.polygonMode = Node.PolygonMode;
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
    rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo.lineWidth = Node.LineWidth;

    VkPipelineColorBlendAttachmentState colorBlendAttachementState = {};
    colorBlendAttachementState.blendEnable = VK_FALSE;
    colorBlendAttachementState.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachementState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachementState.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachementState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachementState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachementState.alphaBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachementState.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {};
    colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
    colorBlendStateCreateInfo.attachmentCount = 1;
    colorBlendStateCreateInfo.pAttachments = &colorBlendAttachementState;
    memset(colorBlendStateCreateInfo.blendConstants, 0, sizeof(float) * 4);

    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = {};
    depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
    depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
    depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;

    depthStencilStateCreateInfo.front.failOp = VK_STENCIL_OP_KEEP;
    depthStencilStateCreateInfo.front.passOp = VK_STENCIL_OP_KEEP;
    depthStencilStateCreateInfo.front.depthFailOp = VK_STENCIL_OP_KEEP;
    depthStencilStateCreateInfo.front.compareOp = VK_COMPARE_OP_ALWAYS;

    depthStencilStateCreateInfo.back = depthStencilStateCreateInfo.front;

    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {};
    multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleStateCreateInfo.rasterizationSamples = D.PhysicalHandleCapabilities.msaaSamples;
    multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;

    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
    graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipelineCreateInfo.stageCount = 2;
    graphicsPipelineCreateInfo.pStages = ShaderStageInfo;
    graphicsPipelineCreateInfo.pVertexInputState = &VertexInputStateInfo;
    graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
    graphicsPipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
    graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
    graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
    graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
    graphicsPipelineCreateInfo.layout = Node.Layout;
    graphicsPipelineCreateInfo.renderPass = Renderpass;
    graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    graphicsPipelineCreateInfo.basePipelineIndex = -1;

    if (vkCreateGraphicsPipelines(D.Handle, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, 0, &Node.Handle)) {
        LogError(GetCurrentLogLocation( ), "Failed to create VkPipeline.");
        return Node;
    }
    return Node;
}

RenderGraph ConstructRenderGraph(const Device& D, const VkRenderPass& Renderpass, const VmaAllocator& Allocator,
                                 JSON::SceneLayout& Layout, const Resource& r) {
    RenderGraph n;

    uint32_t sMax = 0;
    JSON::Dimension Dim;
    for (auto& obj : Layout.MeshArrays) {
        RenderGraphNode tmp = FillRenderGraphNode(Allocator, obj);
        Dim = tmp.BatchDimension;
        n.Nodes.push_back(ConstructRenderGraphNode(D, Renderpass, r, tmp));
    }
    for (auto& Node : n.Nodes)
        sMax = std::max(sMax, (uint32_t)(Node.CPUMeshData.BatchedMeshes.ElementCount * Node.CPUMeshData.BatchedMeshes.ElementSize));
    BufferCreateInfo bCreateInfo = {};
    bCreateInfo.vkData.SharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bCreateInfo.vkData.Size = sMax;
    bCreateInfo.vkData.Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    bCreateInfo.vmaData.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    bCreateInfo.vmaData.Usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    n.PushBuffer = CreateBuffer(Allocator, bCreateInfo);

    if (n.PushBuffer.Handle == VK_NULL_HANDLE)
        return n;

    n.Layout = Layout;
    n.PushCamera.Model = glm::mat4(1.0f);
    n.PushCamera.ViewProj = glm::mat4(1.0f);
    n.Cam.Type = (Dim == JSON::Dimension::Mesh2D) ? CameraType::_2D : CameraType::_3D;
    return n;
}

void DestroyRenderGraph(const VkDevice& Device, const VmaAllocator& Allocator, RenderGraph Graph) {
    for (auto& Node : Graph.Nodes) {
        vkDestroyPipeline(Device, Node.Handle, 0);
        vkDestroyPipelineLayout(Device, Node.Layout, 0);
        DestroyBatch(Node.CPUMeshData);
    }
    DestroyBuffer(Allocator, Graph.PushBuffer);
}

void ReloadRenderGraph(const Device& D, const VkRenderPass& RenderPass, const Resource& r, RenderGraph& Graph) {
    if (!Graph.Update)
        return;
    vkDeviceWaitIdle(D.Handle);
    for (auto& Node : Graph.Nodes) {
        if (Node.Update) {
            if (Node.Handle != VK_NULL_HANDLE && Node.Layout != VK_NULL_HANDLE) {
                vkDestroyPipeline(D.Handle, Node.Handle, 0);
                vkDestroyPipelineLayout(D.Handle, Node.Layout, 0);
                Node.Handle = VK_NULL_HANDLE;
                Node.Layout = VK_NULL_HANDLE;
            }
            ConstructRenderGraphNode(D, RenderPass, r, Node);
            Node.Update = false;
        }
    }
    Graph.Update = false;
}

}    // namespace Vulkan
}    // namespace ffGraph