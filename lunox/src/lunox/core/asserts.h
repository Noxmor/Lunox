#ifndef LNX_ASSERTS_H
#define LNX_ASSERTS_H

#if defined(__clang__) || defined(__GNUC__)
    #define LNX_STATIC_ASSERT(condition, message) _Static_assert(condition, message)
#else
    #define LNX_STATIC_ASSERT(condition, message) static_assert(condition, message)
#endif

#ifdef _MSC_VER
    #include <intrin.h>
    #define LNX_DEBUGBREAK() __debugbreak()
#else
    #define LNX_DEBUGBREAK() __builtin_trap()
#endif

#ifdef LNX_ENABLE_ASSERTS
    #include <stdio.h>
    #define LNX_ASSERT(condition) if(!(condition)) { printf("Assertion failed at %s:%d: \"%s\"\n", __FILE__, __LINE__, #condition); LNX_DEBUGBREAK(); }
#else
    #define LNX_ASSERT(condition)
#endif

#ifdef LNX_ENABLE_VERIFY
    #include <stdio.h>
    #define LNX_VERIFY(condition) if(!(condition)) { printf("Verify failed at %s:%d: \"%s\"\n", __FILE__, __LINE__, #condition); LNX_DEBUGBREAK(); }
#else
    #define LNX_VERIFY(condition)
#endif

#endif