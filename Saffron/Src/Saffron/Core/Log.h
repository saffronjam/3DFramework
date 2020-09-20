#pragma once

#include "Saffron/System/Macros.h"

#include <spdlog/spdlog.h>

namespace Se {

class Log
{
public:
	static void Init();

	static std::shared_ptr<spdlog::logger> &GetCoreLogger() { return s_CoreLogger; }
	static std::shared_ptr<spdlog::logger> &GetClientLogger() { return s_ClientLogger; }
private:
	static std::shared_ptr<spdlog::logger> s_CoreLogger;
	static std::shared_ptr<spdlog::logger> s_ClientLogger;
};
}

// Core Logging Macros
#define SE_CORE_TRACE(...)	Se::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define SE_CORE_INFO(...)	Se::Log::GetCoreLogger()->info(__VA_ARGS__)
#define SE_CORE_WARN(...)	Se::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define SE_CORE_ERROR(...)	Se::Log::GetCoreLogger()->error(__VA_ARGS__)
#define SE_CORE_FATAL(...)	Se::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client Logging Macros
#define SE_TRACE(...)	Se::Log::GetClientLogger()->trace(__VA_ARGS__)
#define SE_INFO(...)	Se::Log::GetClientLogger()->info(__VA_ARGS__)
#define SE_WARN(...)	Se::Log::GetClientLogger()->warn(__VA_ARGS__)
#define SE_ERROR(...)	Se::Log::GetClientLogger()->error(__VA_ARGS__)
#define SE_FATAL(...)	Se::Log::GetClientLogger()->critical(__VA_ARGS__)
