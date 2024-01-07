#pragma once

#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __cplusplus
#define EXTERN_C_BEGIN extern "C" {
#define EXTERN_C_END }
#else
#define EXTERN_C_BEGIN
#define EXTERN_C_END
#endif

#define IS_MSVC _MSC_VER && !__INTEL_COMPILER

#if defined(__GNUC__) && defined(__GNUC_MINOR__) && defined(__GNUC_PATCHLEVEL__)
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#elif defined(__GNUC__) && defined(__GNUC_MINOR__)
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100)
#else
#define GCC_VERSION 0
#endif

#ifdef __GNUC__
#define ATTRIBUTE_NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
#define ATTRIBUTE_NORETURN __declspec(noreturn)
#else
#define ATTRIBUTE_NORETURN
#endif

#ifdef _MSC_VER
#	if _MSC_VER >= 1400
#		include <sal.h>
#		if _MSC_VER > 1400
#			define PRINTFLIKE_FORMAT(__PARAMETER__) _Printf_format_string_ __PARAMETER__
#		else
#			define PRINTFLIKE_FORMAT(__PARAMETER__) __format_string __PARAMETER__
#		endif
#	endif
#else
#	ifndef PRINTFLIKE
#		define PRINTFLIKE(__FORMAT_INDEX__, __VARARG_INDEX__) __attribute__((__format__(__printf__, __FORMAT_INDEX__, __VARARG_INDEX__)))
#	endif
#endif

#ifndef PRINTFLIKE_PARAMETER(__PARAMETER__) __PARAMETER__
#define PRINTFLIKE_PARAMETER(__PARAMETER__) __PARAMETER__
#endif

#ifndef PRINTFLIKE_ATTRIBUTE(__FORMAT_INDEX__, __VARARG_INDEX__)
#define PRINTFLIKE_ATTRIBUTE(__FORMAT_INDEX__, __VARARG_INDEX__)
#endif

ATTRIBUTE_NORETURN void __unreachable(const char* message, const char* file, size_t line);

#ifndef UNREACHABLE
#define UNREACHABLE(__MESSAGE__) __unreachable(__MESSAGE__, __FILE__, __LINE__)
#endif

#ifndef UNIMPLEMENTED
#define UNIMPLEMENTED FatalError("Implementation Missing!")
#endif

#ifndef MIN
#define MIN(__X__, __Y__) (__X__ <= __Y__ ? __X__ : __Y__)
#endif

#ifndef MAX
#define MAX(__X__, __Y__) (__X__ >= __Y__ ? __X__ : __Y__)
#endif

#ifndef ABS
#define ABS(__X__) (((__X__) < 0) ? -(__X__) : (__X__))
#endif

#ifndef CONCAT
#define CONCAT(__LHS__, __RHS__) __LHS__##__RHS__
#endif

#ifndef UNIQUE
#define UNIQUE(__PREFIX__) CONCAT(__PREFIX__, __COUNTER__)
#endif

#if IS_MSVC
#define MSVC_HACK(FUNC, ARGS) FUNC ARGS
#define APPLY(FUNC, ...) MSVC_HACK(FUNC, (__VA_ARGS__))
#define VA_LENGTH(...) APPLY(VA_LENGTH_, 0, ## __VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#else
#define VA_LENGTH(...) VA_LENGTH_(0, ## __VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#endif

#define VA_LENGTH_(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, N, ...) N

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef _WIN32
#define PLATFORM_PATH_SEPARATOR ((char)'\\')
#else
#define PLATFORM_PATH_SEPARATOR ((char)'/')
#endif

typedef void Void;
typedef bool Bool;
typedef char Char;
typedef char* CString;
typedef uint8_t UInt8;
typedef uint16_t UInt16;
typedef uint32_t UInt32;
typedef uint64_t UInt64;
typedef int8_t Int8;
typedef int16_t Int16;
typedef int32_t Int32;
typedef int64_t Int64;
typedef float Float32;
typedef double Float64;
typedef Float64 Float;
typedef UInt64 Timestamp;
typedef UInt8* MemoryRef;

// We use the precompiler macro MOCK to track inlined mock data in runtime code base
#define MOCK