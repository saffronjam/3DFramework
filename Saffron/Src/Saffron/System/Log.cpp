#include "Saffron/SaffronPCH.h"

#include <spdlog/sinks/stdout_color_sinks.h>

#include "Saffron/System/Log.h"

namespace Se
{
std::shared_ptr<spdlog::logger> Log::m_sCoreLogger;
std::shared_ptr<spdlog::logger> Log::m_sClientLogger;

void Log::Init()
{
	spdlog::set_pattern("%^[%T] %n: %v%$");
	m_sCoreLogger = spdlog::stdout_color_mt("Saffron");
	m_sCoreLogger->set_level(spdlog::level::trace);
	m_sClientLogger = spdlog::stdout_color_mt("App");
	m_sClientLogger->set_level(spdlog::level::trace);
}

std::shared_ptr<spdlog::logger> &Log::GetCoreLogger()
{
	return m_sCoreLogger;
}

std::shared_ptr<spdlog::logger> &Log::GetClientLogger()
{
	return m_sClientLogger;
}
}
