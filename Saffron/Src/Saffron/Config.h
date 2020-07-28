#pragma once

#include <memory>

// Platform detection using predefined macros
#ifdef _WIN32
	/* Windows x64/x86 */
#ifdef _WIN64
	/* Windows x64  */
#define SE_PLATFORM_WINDOWS
#else
	/* Windows x86 */
#error "x86 Builds are not supported!"
#endif
#endif


#ifndef SE_PLATFORM_WINDOWS
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

namespace Se
{

template<typename T>
using Scope = std::unique_ptr<T>;
template<typename T, typename ... Args>
constexpr Scope<T> CreateScope(Args && ... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using Ref = std::shared_ptr<T>;
template<typename T, typename ... Args>
constexpr Ref<T> CreateRef(Args && ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

}
