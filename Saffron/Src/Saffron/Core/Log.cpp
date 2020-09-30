#include "SaffronPCH.h"
#include "Log.h"

namespace Se {

std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

void Log::Init()
{
	spdlog::set_pattern("%^[%T] %n: %v%$");
	s_CoreLogger = spdlog::stdout_color_mt("SAFFRON");
	s_CoreLogger->set_level(spdlog::level::trace);

	s_ClientLogger = spdlog::stdout_color_mt("APP");
	s_ClientLogger->set_level(spdlog::level::trace);
	s_ClientLogger->set_pattern("%^[%H:%M:%S]: %v%$");

}

void Log::AddCoreSink(spdlog::sink_ptr sink)
{
	auto &sinks = const_cast<std::vector<spdlog::sink_ptr> &>(s_CoreLogger->sinks());
	sinks.push_back(sink);
}

void Log::AddClientSink(spdlog::sink_ptr sink)
{
	auto &sinks = const_cast<std::vector<spdlog::sink_ptr> &>(s_ClientLogger->sinks());
	sinks.push_back(sink);
}
}
