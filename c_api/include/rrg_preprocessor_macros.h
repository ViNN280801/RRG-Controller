#ifndef RRG_PREPROCESSOR_MACROS_H
#define RRG_PREPROCESSOR_MACROS_H

/**
 * @def RRG_RESTRICT
 * @brief Cross-platform definition of the `restrict` keyword for compiler optimization.
 */
#if defined(_MSC_VER)
#define RRG_RESTRICT __restrict
#elif defined(__GNUC__) || defined(__clang__)
#define RRG_RESTRICT __restrict__
#else
#define RRG_RESTRICT
#endif

/* Cross-platform shared library export macros */
#if defined(_MSC_VER) // Windows (Microsoft Visual Studio)
#if defined(RRG_DLL_EXPORTS)
#define RRG_API __declspec(dllexport)
#else
#define RRG_API __declspec(dllimport)
#endif
#else // Linux/macOS
#define RRG_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
#define RRG_BEGIN_DECLS \
    extern "C"          \
    {
#define RRG_END_DECLS }
#else
#define RRG_BEGIN_DECLS
#define RRG_END_DECLS
#endif

/* Likely/Unlikely branch prediction hints */
#if defined(__GNUC__) || defined(__clang__)
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif

/* Function attributes for optimization */
#if defined(__GNUC__) || defined(__clang__)
#define RRG_HOT __attribute__((hot))
#define RRG_PURE __attribute__((pure))
#define RRG_CONST __attribute__((const))
#elif defined(_MSC_VER)
#define RRG_HOT __forceinline
#define RRG_PURE __declspec(noalias)
#define RRG_CONST __declspec(noalias)
#else
#define RRG_HOT
#define RRG_PURE
#define RRG_CONST
#endif

#endif // !RRG_PREPROCESSOR_MACROS_H
