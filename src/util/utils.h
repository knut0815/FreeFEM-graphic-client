#ifndef UTILS_H
#define UTILS_H

#include <assert.h>
#include <cstdint>
#include <cstdio>
#include "NonCopyable.h"

namespace FEM {

#if defined(__GNUC__) && (__GNUC__ >= 4)
/**
 * @brief Force the compiler to throw a warning if the result of the marked
 * function isn't used. This macro is platform specific.
 */
#define FORCE_USE_RESULT __attribute__((warn_unused_result))

#define UNUSED_PARAM __attribute__((unused))

#elif defined(_MSC_VER) && (_MSC_VER >= 1700)
/**
 * @brief Force the compiler to throw a warning if the result of the marked
 * function isn't used. This macro is platform specific.
 */
#define FORCE_USE_RESULT _Check_return_

#define UNUSED_PARAM
#else
/**
 * @brief Force the compiler to throw a warning if the result of the marked
 * function isn't used. This macro is platform specific.
 */
#define FORCE_USE_RESULT

#define UNUSED_PARAM
#endif

/**
 * @brief Return values
 */
struct ErrorValues {
    /**
     * Error value.
     */
    uint32_t m_value = NONE;

    static constexpr uint32_t NONE = 0;
    static constexpr uint32_t FUNCTION_FAILED = 1;
    static constexpr uint32_t FAILED_ALLOC = 2;
    static constexpr uint32_t FILE_NOT_FOUND = 4;
    static constexpr uint32_t FILE_ACCESS = 5;
    static constexpr uint32_t UNKNOWN = 6;

    /**
     * @brief Error's name as strings.
     */
    const char ErrorNames[6][19] = {"NONE",           "FUNCTION_FAILED",    "FAILED_ALLOC",
                                    "FILE_NOT_FOUND", "FILE_ACCESS_FAILED", "UNKNOWN"};

    /**
     * @brief Default constructor.
     */
    ErrorValues( ) : m_value(ErrorValues::NONE) {}
    /**
     * @brief Error constructor.
     *
     * @param uint32_t code[in] - Error code.
     */
    ErrorValues(const uint32_t code) : m_value(code) {}

    /**
     * @brief Error copy operator
     */
    ErrorValues& operator=(const ErrorValues& b) {
        m_value = b.m_value;
        return *this;
    }

    /**
     * @brief Error equal operator.
     *
     * @param const Error& b[in] - Error to compare.
     */
    bool operator==(const ErrorValues& b) const { return m_value == b.m_value; }

    /**
     * @brief Error equal operator.
     *
     * @param const uint32_t code[in] - Error code to compare.
     */
    bool operator==(const uint32_t code) const { return m_value == code; }

    /**
     * @brief Error different operator.
     *
     * @param const Error& b[in] - Error to compare.
     */
    bool operator!=(const ErrorValues& b) const { return m_value != b.m_value; }

    /**
     * @brief Error different operator.
     *
     * @param const uint32_t code[in] - Error code to compare.
     */
    bool operator!=(const uint32_t code) const { return m_value != code; }

    operator bool( ) const { return m_value != NONE; }
};

}    // namespace FEM

// Axes
// static constexpr int NUM_DEMO_VERTICES = 6;
// static constexpr int SIZE_DEMO_VERTEX = sizeof(float) * 6;
// static const float vertices[] = {
//     0.f, 0.f, 0.f, 1.f, 0.f, 0.f, // RED
//     1.f, 0.f, 0.f, 1.f, 0.f, 0.f, // RED

//     0.f, 0.f, 0.f, 0.f, 1.f, 0.f, // GREEN
//     0.f, 1.f, 0.f, 0.f, 1.f, 0.f, // GREEN

//     0.f, 0.f, 0.f, 0.f, 0.f, 1.f, // BLUE
//     0.f, 0.f, 1.f, 0.f, 0.f, 1.f  // BLUE
// };

// Curve
static constexpr int NUM_DEMO_VERTICES = 200;
static constexpr int SIZE_DEMO_VERTEX = 24;
static const float vertices[] = {
    0.0000,  0.0000,  0.0000,  1.0000,  0.0000,  0.0000,  0.1000,  0.0998,  0.0000,  1.0000,  0.0000,  0.0000,  0.1000,
    0.0998,  0.0000,  1.0000,  0.0000,  0.0000,  0.2000,  0.1987,  0.0000,  1.0000,  0.0000,  0.0000,  0.2000,  0.1987,
    0.0000,  1.0000,  0.0000,  0.0000,  0.3000,  0.2955,  0.0000,  1.0000,  0.0000,  0.0000,  0.3000,  0.2955,  0.0000,
    1.0000,  0.0000,  0.0000,  0.4000,  0.3894,  0.0000,  1.0000,  0.0000,  0.0000,  0.4000,  0.3894,  0.0000,  1.0000,
    0.0000,  0.0000,  0.5000,  0.4794,  0.0000,  1.0000,  0.0000,  0.0000,  0.5000,  0.4794,  0.0000,  1.0000,  0.0000,
    0.0000,  0.6000,  0.5646,  0.0000,  1.0000,  0.0000,  0.0000,  0.6000,  0.5646,  0.0000,  1.0000,  0.0000,  0.0000,
    0.7000,  0.6442,  0.0000,  1.0000,  0.0000,  0.0000,  0.7000,  0.6442,  0.0000,  1.0000,  0.0000,  0.0000,  0.8000,
    0.7174,  0.0000,  1.0000,  0.0000,  0.0000,  0.8000,  0.7174,  0.0000,  1.0000,  0.0000,  0.0000,  0.9000,  0.7833,
    0.0000,  1.0000,  0.0000,  0.0000,  0.9000,  0.7833,  0.0000,  1.0000,  0.0000,  0.0000,  1.0000,  0.8415,  0.0000,
    1.0000,  0.0000,  0.0000,  1.0000,  0.8415,  0.0000,  1.0000,  0.0000,  0.0000,  1.1000,  0.8912,  0.0000,  1.0000,
    0.0000,  0.0000,  1.1000,  0.8912,  0.0000,  1.0000,  0.0000,  0.0000,  1.2000,  0.9320,  0.0000,  1.0000,  0.0000,
    0.0000,  1.2000,  0.9320,  0.0000,  1.0000,  0.0000,  0.0000,  1.3000,  0.9636,  0.0000,  1.0000,  0.0000,  0.0000,
    1.3000,  0.9636,  0.0000,  1.0000,  0.0000,  0.0000,  1.4000,  0.9854,  0.0000,  1.0000,  0.0000,  0.0000,  1.4000,
    0.9854,  0.0000,  1.0000,  0.0000,  0.0000,  1.5000,  0.9975,  0.0000,  1.0000,  0.0000,  0.0000,  1.5000,  0.9975,
    0.0000,  1.0000,  0.0000,  0.0000,  1.6000,  0.9996,  0.0000,  1.0000,  0.0000,  0.0000,  1.6000,  0.9996,  0.0000,
    1.0000,  0.0000,  0.0000,  1.7000,  0.9917,  0.0000,  1.0000,  0.0000,  0.0000,  1.7000,  0.9917,  0.0000,  1.0000,
    0.0000,  0.0000,  1.8000,  0.9738,  0.0000,  1.0000,  0.0000,  0.0000,  1.8000,  0.9738,  0.0000,  1.0000,  0.0000,
    0.0000,  1.9000,  0.9463,  0.0000,  1.0000,  0.0000,  0.0000,  1.9000,  0.9463,  0.0000,  1.0000,  0.0000,  0.0000,
    2.0000,  0.9093,  0.0000,  1.0000,  0.0000,  0.0000,  2.0000,  0.9093,  0.0000,  1.0000,  0.0000,  0.0000,  2.1000,
    0.8632,  0.0000,  1.0000,  0.0000,  0.0000,  2.1000,  0.8632,  0.0000,  1.0000,  0.0000,  0.0000,  2.2000,  0.8085,
    0.0000,  1.0000,  0.0000,  0.0000,  2.2000,  0.8085,  0.0000,  1.0000,  0.0000,  0.0000,  2.3000,  0.7457,  0.0000,
    1.0000,  0.0000,  0.0000,  2.3000,  0.7457,  0.0000,  1.0000,  0.0000,  0.0000,  2.4000,  0.6755,  0.0000,  1.0000,
    0.0000,  0.0000,  2.4000,  0.6755,  0.0000,  1.0000,  0.0000,  0.0000,  2.5000,  0.5985,  0.0000,  1.0000,  0.0000,
    0.0000,  2.5000,  0.5985,  0.0000,  1.0000,  0.0000,  0.0000,  2.6000,  0.5155,  0.0000,  1.0000,  0.0000,  0.0000,
    2.6000,  0.5155,  0.0000,  1.0000,  0.0000,  0.0000,  2.7000,  0.4274,  0.0000,  1.0000,  0.0000,  0.0000,  2.7000,
    0.4274,  0.0000,  1.0000,  0.0000,  0.0000,  2.8000,  0.3350,  0.0000,  1.0000,  0.0000,  0.0000,  2.8000,  0.3350,
    0.0000,  1.0000,  0.0000,  0.0000,  2.9000,  0.2392,  0.0000,  1.0000,  0.0000,  0.0000,  2.9000,  0.2392,  0.0000,
    1.0000,  0.0000,  0.0000,  3.0000,  0.1411,  0.0000,  1.0000,  0.0000,  0.0000,  3.0000,  0.1411,  0.0000,  1.0000,
    0.0000,  0.0000,  3.1000,  0.0416,  0.0000,  1.0000,  0.0000,  0.0000,  3.1000,  0.0416,  0.0000,  1.0000,  0.0000,
    0.0000,  3.2000,  -0.0584, 0.0000,  1.0000,  0.0000,  0.0000,  3.2000,  -0.0584, 0.0000,  1.0000,  0.0000,  0.0000,
    3.3000,  -0.1577, 0.0000,  1.0000,  0.0000,  0.0000,  3.3000,  -0.1577, 0.0000,  1.0000,  0.0000,  0.0000,  3.4000,
    -0.2555, 0.0000,  1.0000,  0.0000,  0.0000,  3.4000,  -0.2555, 0.0000,  1.0000,  0.0000,  0.0000,  3.5000,  -0.3508,
    0.0000,  1.0000,  0.0000,  0.0000,  3.5000,  -0.3508, 0.0000,  1.0000,  0.0000,  0.0000,  3.6000,  -0.4425, 0.0000,
    1.0000,  0.0000,  0.0000,  3.6000,  -0.4425, 0.0000,  1.0000,  0.0000,  0.0000,  3.7000,  -0.5298, 0.0000,  1.0000,
    0.0000,  0.0000,  3.7000,  -0.5298, 0.0000,  1.0000,  0.0000,  0.0000,  3.8000,  -0.6119, 0.0000,  1.0000,  0.0000,
    0.0000,  3.8000,  -0.6119, 0.0000,  1.0000,  0.0000,  0.0000,  3.9000,  -0.6878, 0.0000,  1.0000,  0.0000,  0.0000,
    3.9000,  -0.6878, 0.0000,  1.0000,  0.0000,  0.0000,  4.0000,  -0.7568, 0.0000,  1.0000,  0.0000,  0.0000,  4.0000,
    -0.7568, 0.0000,  1.0000,  0.0000,  0.0000,  4.1000,  -0.8183, 0.0000,  1.0000,  0.0000,  0.0000,  4.1000,  -0.8183,
    0.0000,  1.0000,  0.0000,  0.0000,  4.2000,  -0.8716, 0.0000,  1.0000,  0.0000,  0.0000,  4.2000,  -0.8716, 0.0000,
    1.0000,  0.0000,  0.0000,  4.3000,  -0.9162, 0.0000,  1.0000,  0.0000,  0.0000,  4.3000,  -0.9162, 0.0000,  1.0000,
    0.0000,  0.0000,  4.4000,  -0.9516, 0.0000,  1.0000,  0.0000,  0.0000,  4.4000,  -0.9516, 0.0000,  1.0000,  0.0000,
    0.0000,  4.5000,  -0.9775, 0.0000,  1.0000,  0.0000,  0.0000,  4.5000,  -0.9775, 0.0000,  1.0000,  0.0000,  0.0000,
    4.6000,  -0.9937, 0.0000,  1.0000,  0.0000,  0.0000,  4.6000,  -0.9937, 0.0000,  1.0000,  0.0000,  0.0000,  4.7000,
    -0.9999, 0.0000,  1.0000,  0.0000,  0.0000,  4.7000,  -0.9999, 0.0000,  1.0000,  0.0000,  0.0000,  4.8000,  -0.9962,
    0.0000,  1.0000,  0.0000,  0.0000,  4.8000,  -0.9962, 0.0000,  1.0000,  0.0000,  0.0000,  4.9000,  -0.9825, 0.0000,
    1.0000,  0.0000,  0.0000,  4.9000,  -0.9825, 0.0000,  1.0000,  0.0000,  0.0000,  5.0000,  -0.9589, 0.0000,  1.0000,
    0.0000,  0.0000,  5.0000,  -0.9589, 0.0000,  1.0000,  0.0000,  0.0000,  5.1000,  -0.9258, 0.0000,  1.0000,  0.0000,
    0.0000,  5.1000,  -0.9258, 0.0000,  1.0000,  0.0000,  0.0000,  5.2000,  -0.8835, 0.0000,  1.0000,  0.0000,  0.0000,
    5.2000,  -0.8835, 0.0000,  1.0000,  0.0000,  0.0000,  5.3000,  -0.8323, 0.0000,  1.0000,  0.0000,  0.0000,  5.3000,
    -0.8323, 0.0000,  1.0000,  0.0000,  0.0000,  5.4000,  -0.7728, 0.0000,  1.0000,  0.0000,  0.0000,  5.4000,  -0.7728,
    0.0000,  1.0000,  0.0000,  0.0000,  5.5000,  -0.7055, 0.0000,  1.0000,  0.0000,  0.0000,  5.5000,  -0.7055, 0.0000,
    1.0000,  0.0000,  0.0000,  5.6000,  -0.6313, 0.0000,  1.0000,  0.0000,  0.0000,  5.6000,  -0.6313, 0.0000,  1.0000,
    0.0000,  0.0000,  5.7000,  -0.5507, 0.0000,  1.0000,  0.0000,  0.0000,  5.7000,  -0.5507, 0.0000,  1.0000,  0.0000,
    0.0000,  5.8000,  -0.4646, 0.0000,  1.0000,  0.0000,  0.0000,  5.8000,  -0.4646, 0.0000,  1.0000,  0.0000,  0.0000,
    5.9000,  -0.3739, 0.0000,  1.0000,  0.0000,  0.0000,  5.9000,  -0.3739, 0.0000,  1.0000,  0.0000,  0.0000,  6.0000,
    -0.2794, 0.0000,  1.0000,  0.0000,  0.0000,  6.0000,  -0.2794, 0.0000,  1.0000,  0.0000,  0.0000,  6.1000,  -0.1822,
    0.0000,  1.0000,  0.0000,  0.0000,  6.1000,  -0.1822, 0.0000,  1.0000,  0.0000,  0.0000,  6.2000,  -0.0831, 0.0000,
    1.0000,  0.0000,  0.0000,  6.2000,  -0.0831, 0.0000,  1.0000,  0.0000,  0.0000,  6.3000,  0.0168,  0.0000,  1.0000,
    0.0000,  0.0000,  6.3000,  0.0168,  0.0000,  1.0000,  0.0000,  0.0000,  6.4000,  0.1165,  0.0000,  1.0000,  0.0000,
    0.0000,  6.4000,  0.1165,  0.0000,  1.0000,  0.0000,  0.0000,  6.5000,  0.2151,  0.0000,  1.0000,  0.0000,  0.0000,
    6.5000,  0.2151,  0.0000,  1.0000,  0.0000,  0.0000,  6.6000,  0.3115,  0.0000,  1.0000,  0.0000,  0.0000,  6.6000,
    0.3115,  0.0000,  1.0000,  0.0000,  0.0000,  6.7000,  0.4048,  0.0000,  1.0000,  0.0000,  0.0000,  6.7000,  0.4048,
    0.0000,  1.0000,  0.0000,  0.0000,  6.8000,  0.4941,  0.0000,  1.0000,  0.0000,  0.0000,  6.8000,  0.4941,  0.0000,
    1.0000,  0.0000,  0.0000,  6.9000,  0.5784,  0.0000,  1.0000,  0.0000,  0.0000,  6.9000,  0.5784,  0.0000,  1.0000,
    0.0000,  0.0000,  7.0000,  0.6570,  0.0000,  1.0000,  0.0000,  0.0000,  7.0000,  0.6570,  0.0000,  1.0000,  0.0000,
    0.0000,  7.1000,  0.7290,  0.0000,  1.0000,  0.0000,  0.0000,  7.1000,  0.7290,  0.0000,  1.0000,  0.0000,  0.0000,
    7.2000,  0.7937,  0.0000,  1.0000,  0.0000,  0.0000,  7.2000,  0.7937,  0.0000,  1.0000,  0.0000,  0.0000,  7.3000,
    0.8504,  0.0000,  1.0000,  0.0000,  0.0000,  7.3000,  0.8504,  0.0000,  1.0000,  0.0000,  0.0000,  7.4000,  0.8987,
    0.0000,  1.0000,  0.0000,  0.0000,  7.4000,  0.8987,  0.0000,  1.0000,  0.0000,  0.0000,  7.5000,  0.9380,  0.0000,
    1.0000,  0.0000,  0.0000,  7.5000,  0.9380,  0.0000,  1.0000,  0.0000,  0.0000,  7.6000,  0.9679,  0.0000,  1.0000,
    0.0000,  0.0000,  7.6000,  0.9679,  0.0000,  1.0000,  0.0000,  0.0000,  7.7000,  0.9882,  0.0000,  1.0000,  0.0000,
    0.0000,  7.7000,  0.9882,  0.0000,  1.0000,  0.0000,  0.0000,  7.8000,  0.9985,  0.0000,  1.0000,  0.0000,  0.0000,
    7.8000,  0.9985,  0.0000,  1.0000,  0.0000,  0.0000,  7.9000,  0.9989,  0.0000,  1.0000,  0.0000,  0.0000,  7.9000,
    0.9989,  0.0000,  1.0000,  0.0000,  0.0000,  8.0000,  0.9894,  0.0000,  1.0000,  0.0000,  0.0000,  8.0000,  0.9894,
    0.0000,  1.0000,  0.0000,  0.0000,  8.1000,  0.9699,  0.0000,  1.0000,  0.0000,  0.0000,  8.1000,  0.9699,  0.0000,
    1.0000,  0.0000,  0.0000,  8.2000,  0.9407,  0.0000,  1.0000,  0.0000,  0.0000,  8.2000,  0.9407,  0.0000,  1.0000,
    0.0000,  0.0000,  8.3000,  0.9022,  0.0000,  1.0000,  0.0000,  0.0000,  8.3000,  0.9022,  0.0000,  1.0000,  0.0000,
    0.0000,  8.4000,  0.8546,  0.0000,  1.0000,  0.0000,  0.0000,  8.4000,  0.8546,  0.0000,  1.0000,  0.0000,  0.0000,
    8.5000,  0.7985,  0.0000,  1.0000,  0.0000,  0.0000,  8.5000,  0.7985,  0.0000,  1.0000,  0.0000,  0.0000,  8.6000,
    0.7344,  0.0000,  1.0000,  0.0000,  0.0000,  8.6000,  0.7344,  0.0000,  1.0000,  0.0000,  0.0000,  8.7000,  0.6630,
    0.0000,  1.0000,  0.0000,  0.0000,  8.7000,  0.6630,  0.0000,  1.0000,  0.0000,  0.0000,  8.8000,  0.5849,  0.0000,
    1.0000,  0.0000,  0.0000,  8.8000,  0.5849,  0.0000,  1.0000,  0.0000,  0.0000,  8.9000,  0.5010,  0.0000,  1.0000,
    0.0000,  0.0000,  8.9000,  0.5010,  0.0000,  1.0000,  0.0000,  0.0000,  9.0000,  0.4121,  0.0000,  1.0000,  0.0000,
    0.0000,  9.0000,  0.4121,  0.0000,  1.0000,  0.0000,  0.0000,  9.1000,  0.3191,  0.0000,  1.0000,  0.0000,  0.0000,
    9.1000,  0.3191,  0.0000,  1.0000,  0.0000,  0.0000,  9.2000,  0.2229,  0.0000,  1.0000,  0.0000,  0.0000,  9.2000,
    0.2229,  0.0000,  1.0000,  0.0000,  0.0000,  9.3000,  0.1245,  0.0000,  1.0000,  0.0000,  0.0000,  9.3000,  0.1245,
    0.0000,  1.0000,  0.0000,  0.0000,  9.4000,  0.0248,  0.0000,  1.0000,  0.0000,  0.0000,  9.4000,  0.0248,  0.0000,
    1.0000,  0.0000,  0.0000,  9.5000,  -0.0752, 0.0000,  1.0000,  0.0000,  0.0000,  9.5000,  -0.0752, 0.0000,  1.0000,
    0.0000,  0.0000,  9.6000,  -0.1743, 0.0000,  1.0000,  0.0000,  0.0000,  9.6000,  -0.1743, 0.0000,  1.0000,  0.0000,
    0.0000,  9.7000,  -0.2718, 0.0000,  1.0000,  0.0000,  0.0000,  9.7000,  -0.2718, 0.0000,  1.0000,  0.0000,  0.0000,
    9.8000,  -0.3665, 0.0000,  1.0000,  0.0000,  0.0000,  9.8000,  -0.3665, 0.0000,  1.0000,  0.0000,  0.0000,  9.9000,
    -0.4575, 0.0000,  1.0000,  0.0000,  0.0000,  9.9000,  -0.4575, 0.0000,  1.0000,  0.0000,  0.0000,  10.0000, -0.5440,
    0.0000,  1.0000,  0.0000,  0.0000,
};

#endif    // UTILS_H