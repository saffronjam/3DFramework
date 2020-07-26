#pragma once

#ifdef SE_PLATFORM_WINDOWS

#ifdef SE_BUILD_DLL
#define SAFFRON_API __declspec(dllexport)
#else
#define SAFFRON_API __declspec(dllimport)
#endif

#else
#error Saffron only support Windows!
#endif

#ifdef SE_DEBUG
#if defined(SE_PLATFORM_WINDOWS)
#define SE_DEBUGBREAK() __debugbreak()
#elif defined(SE_PLATFORM_LINUX)
#include <signal.h>
#define SE_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform doesn't support debugbreak yet!"
#endif
#endif

#ifdef SE_ENABLE_ASSERTS
#define SE_ASSERT(x, ...) { if(!(x)) { SE_ERROR("Assertion Failed: {0}", __VA_ARGS__); SE_DEBUGBREAK(); } }
#define SE_CORE_ASSERT(x, ...) { if(!(x)) { SE_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); SE_DEBUGBREAK(); } }
#else
#define SE_ASSERT(x, ...)
#define SE_CORE_ASSERT(x, ...)
#endif