#ifndef __RENDER_UTIL_H__
#define __RENDER_UTIL_H__
#include "assert.h"

#define ASSERT(x, message) assert(x || message)


#define LOG(...) \
    ::fprintf(stdout, __FILE__, __FUNCTION__,__LINE__);             \
    ::fprintf(stdout, ": ");                                         \
    ::fprintf(stdout, __VA_ARGS__);                                 \
    ::fprintf(stdout, "\n");                                        \
    ::fflush(stdout)




const int width  = 800;
const int height = 800;
#endif
