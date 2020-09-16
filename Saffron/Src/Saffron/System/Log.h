#pragma once

#include <spdlog/spdlog.h>

namespace Se
{

class Log
{
public:
	static void Init();

	static std::shared_ptr<spdlog::logger> &GetCoreLogger();
	static std::shared_ptr<spdlog::logger> &GetClientLogger();

private:
	static std::shared_ptr<spdlog::logger> m_sCoreLogger;
	static std::shared_ptr<spdlog::logger> m_sClientLogger;

};

// Core Logging Macros
#define SE_CORE_TRACE(...)		::Se::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define SE_CORE_INFO(...)		::Se::Log::GetCoreLogger()->info(__VA_ARGS__)
#define SE_CORE_WARN(...)		::Se::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define SE_CORE_ERROR(...)		::Se::Log::GetCoreLogger()->error(__VA_ARGS__)
#define SE_CORE_CRITICAL(...)	::Se::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client Logging Macros
#define SE_TRACE(...)			::Se::Log::GetClientLogger()->trace(__VA_ARGS__)
#define SE_INFO(...)			::Se::Log::GetClientLogger()->info(__VA_ARGS__)
#define SE_WARN(...)			::Se::Log::GetClientLogger()->warn(__VA_ARGS__)
#define SE_ERROR(...)			::Se::Log::GetClientLogger()->error(__VA_ARGS__)
#define SE_CRITICAL(...)		::Se::Log::GetClientLogger()->critical(__VA_ARGS__)

}
