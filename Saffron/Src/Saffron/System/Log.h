#pragma once

#include <memory>

#include <spdlog/spdlog.h>

#include "Saffron/Config.h"

namespace Saffron
{

class SAFFRON_API Log
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
#define SE_CORE_TRACE(...) ::Saffron::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define SE_CORE_INFO(...)  ::Saffron::Log::GetCoreLogger()->info(__VA_ARGS__)
#define SE_CORE_WARN(...)  ::Saffron::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define SE_CORE_ERROR(...) ::Saffron::Log::GetCoreLogger()->error(__VA_ARGS__)

// Client Logging Macros
#define SE_TRACE(...)      ::Saffron::Log::GetClientLogger()->trace(__VA_ARGS__)
#define SE_INFO(...)       ::Saffron::Log::GetClientLogger()->info(__VA_ARGS__)
#define SE_WARN(...)       ::Saffron::Log::GetClientLogger()->warn(__VA_ARGS__)
#define SE_ERROR(...)      ::Saffron::Log::GetClientLogger()->error(__VA_ARGS__)

}

