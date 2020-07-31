﻿#include "Saffron/SaffronPCH.h"
#include "Saffron/System/Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace Se
{

Ref<spdlog::logger> Log::m_sCoreLogger;
Ref<spdlog::logger> Log::m_sClientLogger;

void Log::Init()
{
	spdlog::set_pattern("%^[%T] %n: %v%$");
	m_sCoreLogger = spdlog::stdout_color_mt("Saffron");
	m_sCoreLogger->set_level(spdlog::level::trace);
	m_sClientLogger = spdlog::stdout_color_mt("App");
	m_sClientLogger->set_level(spdlog::level::trace);
}

Ref<spdlog::logger> &Log::GetCoreLogger()
{
	return m_sCoreLogger;
}

Ref<spdlog::logger> &Log::GetClientLogger()
{
	return m_sClientLogger;
}
}
