#pragma once
// http://www.cplusplus.com/reference/cassert/assert/
// #define NDEBUG to disable assertions
#include <cassert>

#if defined(GLSLASSEMBLER_BUILD)
    #if defined(GLSLASSEMBLER_SHARED)
        #define GLSLASSEMBLER_API __declspec(dllexport)
    #else
        #define GLSLASSEMBLER_API
    #endif
#else
    #if defined(GLSLASSEMBLER_SHARED)
        #define GLSLASSEMBLER_API __declspec(dllimport)
    #else
        #define GLSLASSEMBLER_API
    #endif
#endif
