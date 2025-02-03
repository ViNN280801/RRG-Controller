#ifndef RRG_PREPROCESSOR_MACROS_H
#define RRG_PREPROCESSOR_MACROS_H

/* Cross-platform definition of the `restrict` keyword for compiler optimization. */
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

#ifdef __linux__
#define RRG_COMMON_PRETTY_FUNC __PRETTY_FUNCTION__
#elif defined(_WIN32)
#define RRG_COMMON_PRETTY_FUNC __FUNCSIG__
#endif

#ifdef RRG_DEBUG
#define RRG_MODBUS_DEBUG_MSG fprintf(stderr, "RRG DEBUG: %s: %s\n", RRG_COMMON_PRETTY_FUNC, modbus_strerror(errno));
#define RRG_DEBUG_MSG(msg) fprintf(stderr, "RRG DEBUG: %s: %s\n", RRG_COMMON_PRETTY_FUNC, msg);
#define RRG_DEBUG_GET_LAST_ERR fprintf(stderr, RRG_GetLastError());
#else
#define RRG_MODBUS_DEBUG_MSG
#define RRG_DEBUG_MSG(msg)
#define RRG_DEBUG_GET_LAST_ERR
#endif

#endif // !RRG_PREPROCESSOR_MACROS_H
