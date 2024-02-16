#ifndef __RENDER_UTIL_H__
#define __RENDER_UTIL_H__
#include "assert.h"
#include <numbers>
#include <stdio.h>
#include <stdlib.h>

#define ASSERT(x, message) assert(x || message)
static FILE* fp = NULL;
#define LOG(...) \
    ::fprintf(stdout, __FILE__, __FUNCTION__,__LINE__);             \
    ::fprintf(stdout, ": ");                                         \
    ::fprintf(stdout, __VA_ARGS__);                                 \
    ::fprintf(stdout, "\n");                                        \
    ::fflush(stdout)


#define LOG_ERROR(...) \
    output(__FILE__, __FUNCTION__, __LINE__);   \
    fprintf(fp, __VA_ARGS__);   \
    fprintf(fp, "\n"); \
    fflush(fp);

static void output(const char* file, const char* function, int line) {
    if(fp == NULL) fp = fopen("output.log", "w");
    fprintf(fp, "%s-%s-%d: ", file, function, line);
}

// inline double random_double() {
//     static std::uniform_real_distribution<double> distribution(0.0, 1.0);
//     static std::mt19937 generator;
//     return distribution(generator);
// }

// 返回[0,1)
inline double random_double() {
    return rand() / (RAND_MAX + 1.0);
}

// 返回[min, max)
inline double random_double(double min, double max) {
    return min + (max - min) * random_double();
}

inline int random_int(double min, double max) {
    return static_cast<int>(random_double(min,max + 1));
}


// 角度转弧度
inline double deg2rad(const double deg) {
    return deg * std::numbers::pi / 180.0;
}
extern const int width;
extern const int height;
extern const int depth;


#endif
