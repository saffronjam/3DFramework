#pragma once

#include "Saffron/Core/TypeDefs.h"

namespace Se
{
class Debug
{
public:

#ifdef SE_DEBUG
	template <typename ... Args>
	static void Assert(bool what, const String& message, Args&& ... formatArgs)
	{
		if (!what)
		{
			//Se::Log::GetCoreLogger()->trace(message, std::forward<Args>(formatArgs)...);
			SE_DEBUGBREAK();
		}
	}

	static void Assert(bool what, const String& message)
	{
		if (!what)
		{
			SE_CORE_TRACE("Assertion failed!", message);
			SE_DEBUGBREAK();
		}
	}

	static void Assert(bool what)
	{
		Assert(what, "");
	}

#else
	template <typename T, typename ... Args>
	static constexpr void Assert(bool what, Args&& ... args)
	{
	}
#endif
};
}
