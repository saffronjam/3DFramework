#include "SaffronPCH.h"

#include "Saffron/Log/Log.h"

namespace Se
{
Log* Log::_instance = nullptr;

Log::Log()
{
	if (_instance != nullptr)
	{
	}
}

void Log::Info(const std::string& string)
{
	std::cout << "[Info] " << string << '\n';
}

void Log::Warn(const std::string& string)
{
	std::cout << "[Warn] " << string << '\n';
}

void Log::Debug(const std::string& string)
{
	std::cerr << "[Debug] " << string << '\n';
}
}
