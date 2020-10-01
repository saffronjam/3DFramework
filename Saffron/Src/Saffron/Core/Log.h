#pragma once

#include <mutex>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/base_sink.h>

#include "Saffron/System/Macros.h"


namespace Se {

class Log
{
public:
	struct Level
	{
		enum LevelEnum
		{
			Trace = 0,
			Debug = 1,
			Info = 2,
			Warn = 3,
			Err = 4,
			Critical = 5,
			Off = 6
		};
	};

public:
	static void Init();

	static void AddCoreSink(std::shared_ptr<class LogSink> sink);
	static void AddClientSink(std::shared_ptr<class LogSink> sink);

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

