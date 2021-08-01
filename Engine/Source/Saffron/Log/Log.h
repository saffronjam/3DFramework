#pragma once

#include <format>

namespace Se
{
class Log
{
public:
	Log();

	template <typename ... Args>
	static void Info(const std::string& string, const Args& ... args);
	static void Info(const std::string& string);
	
	template <typename ... Args>
	static void Debug(const std::string& string, const Args& ... args);
	static void Debug(const std::string& string);
	
private:
	static Log* _instance;
};

template <typename ... Args>
void Log::Info(const std::string& string, const Args&... args)
{
	Info(std::format(string, args...));
}

template <typename ... Args>
void Log::Debug(const std::string& string, const Args&... args)
{
	Debug(std::format(string, args...));
}

class UsrLog
{
};

}
