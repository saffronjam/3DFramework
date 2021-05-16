#include "SaffronPCH.h"

#include "Saffron/Core/Log.h"
#include "Saffron/Core/LogSink.h"

namespace Se
{
Log* Log::_instance = nullptr;

void Log::Init()
{
	_instance = new Log();

	spdlog::set_pattern("%^[%T] %n: %v%$");
	Instance()._coreLogger = spdlog::stdout_color_mt("Saffron");
	Instance()._coreLogger->set_level(spdlog::level::trace);

	Instance()._clientLogger.reset(new spdlog::logger("App", {}));
	Instance()._clientLogger->set_pattern("%^[%H:%M:%S]: %v%$");
}

void Log::AddCoreSink(std::shared_ptr<LogSink> sink)
{
	auto& sinks = const_cast<List<spdlog::sink_ptr>&>(Instance()._coreLogger->sinks());
	sinks.push_back(std::dynamic_pointer_cast<spdlog::sinks::sink>(sink));
}


void Log::AddClientSink(std::shared_ptr<LogSink> sink)
{
	auto& sinks = const_cast<List<spdlog::sink_ptr>&>(Instance()._clientLogger->sinks());
	sinks.push_back(std::reinterpret_pointer_cast<spdlog::sinks::sink>(sink));
}

bool Log::IsInitialized()
{
	return _instance != nullptr;
}

Log& Log::Instance()
{
	assert(_instance != nullptr);
	return *_instance;
}

std::shared_ptr<spdlog::logger>& Log::GetCoreLogger()
{
	return Instance()._coreLogger;
}

std::shared_ptr<spdlog::logger>& Log::GetClientLogger()
{
	return Instance()._clientLogger;
}
}
