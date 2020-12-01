#include "SaffronPCH.h"

#include "Saffron/Core/LogSink.h"

namespace Se
{
void LogSink::SetLevel(Log::Level::LevelEnum level)
{
	set_level(static_cast<spdlog::level::level_enum>(level));
}
}
