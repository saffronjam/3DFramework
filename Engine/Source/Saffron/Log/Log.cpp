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
	std::cout << string << '\n';
}

void Log::Debug(const std::string& string)
{
	std::cerr << string << '\n';
}
}
