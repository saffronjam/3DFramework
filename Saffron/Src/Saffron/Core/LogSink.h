#pragma once

#include "Saffron/Base.h"
#include "Saffron/Core/Log.h"
#include "Saffron/Core/LogMessage.h"

namespace Se
{
class LogSink : protected spdlog::sinks::sink
{
	friend class Log;

public:
	virtual void Sink(const LogMessage &message) = 0;
	virtual void Flush() = 0;

	void SetLevel(Log::Level::LevelEnum level);

	operator sink *() { return this; }

	const LogSink &operator()(const LogSink &sink);

protected:
	void log(const spdlog::details::log_msg &msg) final override
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		Sink(LogMessage(msg));
	}

	void flush() final override
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		Flush();
	}

private:
	std::mutex m_Mutex;
};
}