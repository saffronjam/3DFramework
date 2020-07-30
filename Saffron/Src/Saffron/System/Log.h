#pragma once

#include <spdlog/spdlog.h>

#include "Saffron/SaffronPCH.h"

namespace Se
{

class Log
{
public:
	using Ptr = ::std::shared_ptr<spdlog::logger>;
public:
	static void Init();

	inline static Ptr &GetCoreLogger()
	{
		return m_sCoreLogger;
	}
	inline static Ptr &GetClientLogger()
	{
		return m_sClientLogger;
	}

private:
	static Ptr m_sCoreLogger;
	static Ptr m_sClientLogger;

};

// Core Logging Macros
#define SE_CORE_TRACE(...) ::Se::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define SE_CORE_INFO(...)  ::Se::Log::GetCoreLogger()->info(__VA_ARGS__)
#define SE_CORE_WARN(...)  ::Se::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define SE_CORE_ERROR(...) ::Se::Log::GetCoreLogger()->error(__VA_ARGS__)

// Client Logging Macros
#define SE_TRACE(...)      ::Se::Log::GetClientLogger()->trace(__VA_ARGS__)
#define SE_INFO(...)       ::Se::Log::GetClientLogger()->info(__VA_ARGS__)
#define SE_WARN(...)       ::Se::Log::GetClientLogger()->warn(__VA_ARGS__)
#define SE_ERROR(...)      ::Se::Log::GetClientLogger()->error(__VA_ARGS__)

}
