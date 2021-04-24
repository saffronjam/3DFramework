#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include "Saffron/Core/Memory.h"
#include "Saffron/Math/SaffronMath.h"

namespace Se
{
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

	void AddCoreSink(std::shared_ptr<class LogSink> sink);
	void AddClientSink(std::shared_ptr<class LogSink> sink);

	static bool IsInitialized();

	template <typename Arg, typename... Args>
	static void CoreInfo(const String& message, Arg&& arg, Args&& ... args);
	template <typename T>
	static void CoreInfo(const T& item);

	template <typename Arg, typename... Args>
	static void CoreTrace(const String& message, Arg&& arg, Args&& ... args);
	template <typename T>
	static void CoreTrace(const T& item);

	template <typename Arg, typename... Args>
	static void CoreDebug(const String& message, Arg&& arg, Args&& ... args);
	template <typename T>
	static void CoreDebug(const T& item);

	template <typename Arg, typename... Args>
	static void CoreWarn(const String& message, Arg&& arg, Args&& ... args);
	template <typename T>
	static void CoreWarn(const T& item);

	template <typename Arg, typename... Args>
	static void CoreError(const String& message, Arg&& arg, Args&& ... args);
	template <typename T>
	static void CoreError(const T& item);

	template <typename Arg, typename... Args>
	static void CoreFatal(const String& message, Arg&& arg, Args&& ... args);
	template <typename T>
	static void CoreFatal(const T& item);

	template <typename Arg, typename... Args>
	static void Info(const String& message, Arg&& arg, Args&& ... args);
	template <typename T>
	static void Info(const T& item);

	template <typename Arg, typename... Args>
	static void Trace(const String& message, Arg&& arg, Args&& ... args);
	template <typename T>
	static void Trace(const T& item);

	template <typename Arg, typename... Args>
	static void Debug(const String& message, Arg&& arg, Args&& ... args);
	template <typename T>
	static void Debug(const T& item);

	template <typename Arg, typename... Args>
	static void Warn(const String& message, Arg&& arg, Args&& ... args);
	template <typename T>
	static void Warn(const T& item);

	template <typename Arg, typename... Args>
	static void Error(const String& message, Arg&& arg, Args&& ... args);
	template <typename T>
	static void Error(const T& item);

	template <typename Arg, typename... Args>
	static void Fatal(const String& message, Arg&& arg, Args&& ... args);
	template <typename T>
	static void Fatal(const T& item);

	static Log& Instance();

private:
	static std::shared_ptr<spdlog::logger>& GetCoreLogger();
	static std::shared_ptr<spdlog::logger>& GetClientLogger();

private:
	static Log* _instance;

	std::shared_ptr<spdlog::logger> _coreLogger;
	std::shared_ptr<spdlog::logger> _clientLogger;
};

template <typename Arg, typename ... Args>
void Log::CoreInfo(const String& message, Arg&& arg, Args&&... args)
{
	GetCoreLogger()->info(message.c_str(), std::forward<Arg>(arg), std::forward<Args>(args)...);
}

template <typename T>
void Log::CoreInfo(const T& item)
{
	GetCoreLogger()->info(item);
}

template <typename Arg, typename ... Args>
void Log::CoreTrace(const String& message, Arg&& arg, Args&&... args)
{
	GetCoreLogger()->trace(message.c_str(), std::forward<Arg>(arg), std::forward<Args>(args)...);
}

template <typename T>
void Log::CoreTrace(const T& item)
{
	GetCoreLogger()->trace(item);
}

template <typename Arg, typename ... Args>
void Log::CoreDebug(const String& message, Arg&& arg, Args&&... args)
{
	GetCoreLogger()->debug(message.c_str(), std::forward<Arg>(arg), std::forward<Args>(args)...);
}

template <typename T>
void Log::CoreDebug(const T& item)
{
	GetCoreLogger()->debug(item);
}

template <typename Arg, typename ... Args>
void Log::CoreWarn(const String& message, Arg&& arg, Args&&... args)
{
	GetCoreLogger()->warn(message.c_str(), std::forward<Arg>(arg), std::forward<Args>(args)...);
}

template <typename T>
void Log::CoreWarn(const T& item)
{
	GetCoreLogger()->warn(item);
}

template <typename Arg, typename ... Args>
void Log::CoreError(const String& message, Arg&& arg, Args&&... args)
{
	GetCoreLogger()->error(message.c_str(), std::forward<Arg>(arg), std::forward<Args>(args)...);
}

template <typename T>
void Log::CoreError(const T& item)
{
	GetCoreLogger()->error(item);
}

template <typename Arg, typename ... Args>
void Log::CoreFatal(const String& message, Arg&& arg, Args&&... args)
{
	GetCoreLogger()->critical(message.c_str(), std::forward<Arg>(arg), std::forward<Args>(args)...);
}

template <typename T>
void Log::CoreFatal(const T& item)
{
	GetCoreLogger()->critical(item);
}

template <typename Arg, typename ... Args>
void Log::Info(const String& message, Arg&& arg, Args&&... args)
{
	GetClientLogger()->info(message.c_str(), std::forward<Arg>(arg), std::forward<Args>(args)...);
}

template <typename T>
void Log::Info(const T& item)
{
	GetClientLogger()->info(item);
}

template <typename Arg, typename ... Args>
void Log::Trace(const String& message, Arg&& arg, Args&&... args)
{
	GetClientLogger()->trace(message.c_str(), std::forward<Arg>(arg), std::forward<Args>(args)...);
}

template <typename T>
void Log::Trace(const T& item)
{
	GetClientLogger()->trace(item);
}

template <typename Arg, typename ... Args>
void Log::Debug(const String& message, Arg&& arg, Args&&... args)
{
	GetClientLogger()->debug(message.c_str(), std::forward<Arg>(arg), std::forward<Args>(args)...);
}

template <typename T>
void Log::Debug(const T& item)
{
	GetClientLogger()->debug(item);
}

template <typename Arg, typename ... Args>
void Log::Warn(const String& message, Arg&& arg, Args&&... args)
{
	GetClientLogger()->warn(message.c_str(), std::forward<Arg>(arg), std::forward<Args>(args)...);
}

template <typename T>
void Log::Warn(const T& item)
{
	GetClientLogger()->warn(item);
}

template <typename Arg, typename ... Args>
void Log::Error(const String& message, Arg&& arg, Args&&... args)
{
	GetClientLogger()->error(message.c_str(), std::forward<Arg>(arg), std::forward<Args>(args)...);
}

template <typename T>
void Log::Error(const T& item)
{
	GetClientLogger()->error(item);
}

template <typename Arg, typename ... Args>
void Log::Fatal(const String& message, Arg&& arg, Args&&... args)
{
	GetClientLogger()->critical(message.c_str(), std::forward<Arg>(arg), std::forward<Args>(args)...);
}

template <typename T>
void Log::Fatal(const T& item)
{
	GetClientLogger()->critical(item);
}
}

template <typename OStream, typename t_Number>
OStream& operator<<(OStream& os, const Se::Vector<2, t_Number>& vec)
{
	return os << '(' << vec.x << ", " << vec.y << ')';
}

template <typename OStream, typename t_Number>
OStream& operator<<(OStream& os, const Se::Vector<3, t_Number>& vec)
{
	return os << '(' << vec.x << ", " << vec.y << ", " << vec.z << ')';
}

template <typename OStream, typename t_Number>
OStream& operator<<(OStream& os, const Se::Vector<4, t_Number>& vec)
{
	return os << '(' << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ')';
}
