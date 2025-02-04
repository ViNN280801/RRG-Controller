#ifndef RELAY_PREPROCESSOR_MACROS_H
#define RELAY_PREPROCESSOR_MACROS_H

/* Cross-platform definition of the `restrict` keyword for compiler optimization. */
#if defined(_MSC_VER)
#define RELAY_RESTRICT __restrict
#elif defined(__GNUC__) || defined(__clang__)
#define RELAY_RESTRICT __restrict__
#else
#define RELAY_RESTRICT
#endif

/* Cross-platform shared library export macros */
#if defined(_MSC_VER) // Windows (Microsoft Visual Studio)
#if defined(RELAY_DLL_EXPORTS)
#define RELAY_API __declspec(dllexport)
#else
#define RELAY_API __declspec(dllimport)
#endif
#else // Linux/macOS
#define RELAY_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
#define RELAY_BEGIN_DECLS \
    extern "C"            \
    {
#define RELAY_END_DECLS }
#else
#define RELAY_BEGIN_DECLS
#define RELAY_END_DECLS
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
#define RELAY_HOT __attribute__((hot))
#define RELAY_PURE __attribute__((pure))
#define RELAY_CONST __attribute__((const))
#elif defined(_MSC_VER)
#define RELAY_HOT __forceinline
#define RELAY_PURE __declspec(noalias)
#define RELAY_CONST __declspec(noalias)
#else
#define RELAY_HOT
#define RELAY_PURE
#define RELAY_CONST
#endif

#ifdef __linux__
#define RELAY_COMMON_PRETTY_FUNC __PRETTY_FUNCTION__
#elif defined(_WIN32)
#define RELAY_COMMON_PRETTY_FUNC __FUNCSIG__
#else
#define RELAY_COMMON_PRETTY_FUNC __func__
#endif

#define RELAY_STRINGIFY(x) RELAY_STRINGIFY_IMPL(x)
#define RELAY_STRINGIFY_IMPL(x) #x

#define RELAY_DEBUG_FMT "RELAY DEBUG: [File: %s, Line: %d, Function: %s]"
#define RELAY_DEBUG_ARGS __FILE__, __LINE__, RELAY_COMMON_PRETTY_FUNC

#ifdef RELAY_DEBUG
#define RELAY_MODBUS_DEBUG_MSG fprintf(stderr, RELAY_DEBUG_FMT ": %s\n", RELAY_DEBUG_ARGS, modbus_strerror(errno));
#define RELAY_DEBUG_MSG(msg) fprintf(stderr, RELAY_DEBUG_FMT ": %s\n", RELAY_DEBUG_ARGS, msg);
#define RELAY_DEBUG_GET_LAST_ERR fprintf(stderr, RELAY_DEBUG_FMT ": %s\n", RELAY_DEBUG_ARGS, RELAY_GetLastError());
#else
#define RELAY_MODBUS_DEBUG_MSG
#define RELAY_DEBUG_MSG(msg)
#define RELAY_DEBUG_GET_LAST_ERR
#endif

#endif // !RELAY_PREPROCESSOR_MACROS_H
