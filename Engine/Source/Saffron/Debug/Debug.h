#pragma once

#include "Saffron/Core/TypeDefs.h"

namespace Se
{
class Debug
{
public:

#ifdef SE_DEBUG
	template <typename Arg, typename... Args>
	static constexpr void Assert(bool what, const String& message, Arg&& arg, Args&& ... args)
	{
		if (!what)
		{
			Log::CoreDebug(AssertionFailed + message, std::forward<Arg>(arg), std::forward<Args>(args)...);
			SE_DEBUGBREAK();
		}
	}

	static constexpr void Assert(bool what, const String& message)
	{
		if (!what)
		{
			Log::CoreDebug(AssertionFailed + message);
			SE_DEBUGBREAK();
		}
	}

	static constexpr void Assert(bool what)
	{
		Assert(what, "");
	}

	template <typename Arg, typename... Args>
	static constexpr void Break(const String& message, Arg&& arg, Args&& ... args)
	{
		Assert(false, message, std::forward<Arg>(arg), std::forward<Args>(args)...);
	}

	static constexpr void Break(const String& message)
	{
		Assert(false, message);
	}

	static constexpr void Break()
	{
		Assert(false, "");
	}

#else
	template <typename T, typename ... Args>
	static constexpr void Assert(bool what, Args&& ... args)
	{
	}
#endif

private:
	static constexpr const char* AssertionFailed = "\033[41m\033[37mAssertion failed\033[m\033[36m ";
};
}
