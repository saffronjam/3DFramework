#pragma once

#define BIT(x) (1 << x)
#define TO_STRING(Var) #Var
#define SE_FUNCTION(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
#define SE_EXT_EVENT_FN(obj, fn) [this](auto&&... args) -> decltype(auto) { return (obj).fn(std::forward<decltype(args)>(args)...); }

#ifdef SE_DEBUG

#if defined(SE_PLATFORM_WINDOWS)
#define SE_DEBUGBREAK() __debugbreak()
#elif defined(SE_PLATFORM_LINUX)
#include <signal.h>
#define SE_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform doesn't support debugbreak yet!"
#endif

// Macros only in debug
#define SE_FN_DBG_ONLY(fn) {fn;}

#else

#define SE_FN_DBG_ONLY(fn)

#endif
