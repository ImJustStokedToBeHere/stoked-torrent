#pragma once
#if !defined(API_DEF)
    #define API_DEF

    #if defined(EXPORT)
        #if defined(_MSC_VER)
            #define API __declspec(dllexport)
        #elif defined(__GNUC__)
            #define API __attribute__((visibility("default")))
        #endif
    #elif defined(IMPORT)
        #if defined(_MSC_VER)
            #define API __declspec(dllimport)
        #else
            #define API
        #endif
    #else
        #define API
    #endif
#endif

#if defined(RELEASE)
    #define INLINE inline
#else
    #define INLINE
#endif

// Check windows
#if defined(_MSC_VER)
    #if defined(_WIN64)
        #define ENV64
    #else
        #define ENV32
    #endif
#endif

// Check GCC
#if defined(__GNUC__) || defined(__clang__)
    #if defined(_LP64) || defined(__x86_64__)
        #define ENV64
    #else
        #define ENV32
    #endif
#endif
