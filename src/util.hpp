#ifndef __RENDER_UTIL_H__
#define __RENDER_UTIL_H__
#include "assert.h"
#include <stdio.h>

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

extern const int width;
extern const int height;
extern const int depth;


template<typename T>
T mix(T x, T y, double a) {
    return x * (1 - a) + y * a;
}

inline double clamp(double v, double x, double y) {
    return std::min(std::max(v, x), y);
}

inline render::Vec3 reflect(render::Vec3 l, render::Vec3 n) {
    return (2 * n * (n * l) - l).normalized();
}
#endif
