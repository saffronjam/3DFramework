#include "SaffronPCH.h"

#include "Instrumentor.h"

#include <iostream>
#include <ostream>
#include <sstream>
#include <iomanip>

#ifndef ENABLE_PROFILE
static bool wroteErrorMessage = false;
#endif

void Instrumentor::BeginSession(const std::string& name, const std::string& filepath)
{
#ifndef ENABLE_PROFILE
	if (!wroteErrorMessage)
	{
		std::cerr <<
			"Tried to start instrumentor session with profiling disabled. Define 'ENABLE_PROFILE' to enable profiling"
			<< std::endl;
		wroteErrorMessage = true;
	}
	return;
#endif

	std::lock_guard lock(_mutex);
	if (_currentSession)
	{
		std::cerr << "Tried to start session '" << name << "' when '" << _currentSession->Name << "' was already open."
			<< std::endl;
		InternalEndSession();
	}
	_outputStream.open(filepath);

	if (_outputStream.is_open())
	{
		_currentSession = new InstrumentationSession({name});
		WriteHeader();
	}
	else
	{
		std::cerr << "Instrumentor could not open results file '" << filepath << "'." << std::endl;
	}
}

void Instrumentor::EndSession()
{
	std::lock_guard lock(_mutex);
	InternalEndSession();
}

void Instrumentor::WriteProfile(const ProfileResult& result)
{
#ifndef ENABLE_PROFILE
	if (!wroteErrorMessage)
	{
		std::cerr << "Tried to write profile with profiling disabled. Define 'ENABLE_PROFILE' to enable profiling" <<
			std::endl;
		wroteErrorMessage = true;
	}
	return;
#endif

	std::ostringstream json;

	json << std::setprecision(20) << std::fixed;
	json << R"(,{)";
	json << R"("cat":"function",)";
	json << R"("dur":)" << static_cast<double>(result.ElapsedTime.count()) / 1000.0f << R"(,)";
	json << R"("name":")" << result.Name << R"(",)";
	json << R"("ph":"X",)";
	json << R"("pid":0,)";
	json << R"("tid":)" << result.ThreadID << ",";
	json << R"("ts":)" << static_cast<double>(result.Start.count()) / 1000.0f;
	json << R"(})";

	std::lock_guard lock(_mutex);
	if (_currentSession)
	{
		_outputStream << json.str();
		_outputStream.flush();
	}
}

Instrumentor& Instrumentor::Get()
{
	static Instrumentor instance;
	return instance;
}

Instrumentor::Instrumentor() :
	_currentSession(nullptr)
{
}

Instrumentor::~Instrumentor()
{
	EndSession();
}

void Instrumentor::WriteHeader()
{
	_outputStream << R"({"otherData": {},"traceEvents":[{})";
	_outputStream.flush();
}

void Instrumentor::WriteFooter()
{
	_outputStream << "]}";
	_outputStream.flush();
}

void Instrumentor::InternalEndSession()
{
	if (_currentSession)
	{
		WriteFooter();
		_outputStream.close();
		delete _currentSession;
		_currentSession = nullptr;
	}
}

void Profiler::BeginSession(const std::string& name, const std::string& filepath)
{
#ifdef ENABLE_PROFILE
	Instrumentor::Get().BeginSession(name, filepath);
#endif
}

void Profiler::EndSession()
{
#ifdef ENABLE_PROFILE
	Instrumentor::Get().EndSession();
#endif
}

void Profiler::Function(const char* functionName)
{
#ifdef ENABLE_PROFILE
	Scope(functionName);
#endif
}

InstrumentationTimer Profiler::Scope(const char* name)
{
	return InstrumentationTimer(name);
}

InstrumentationTimer::InstrumentationTimer(const char* name) :
	_name(name),
	_stopped(false)
{
	_last = std::chrono::high_resolution_clock::now();
}

InstrumentationTimer::~InstrumentationTimer()
{
#ifdef ENABLE_PROFILE
	if (!_stopped)
	{
		Stop();
	}
#endif
}

void InstrumentationTimer::Stop()
{
#ifdef ENABLE_PROFILE
	try
	{
		using step = std::chrono::nanoseconds;

		const auto endTimepoint = std::chrono::high_resolution_clock::now();
		const auto highResStart = ProfileResult::Duration{_last.time_since_epoch()};
		const auto elapsedTime = std::chrono::time_point_cast<step>(endTimepoint).time_since_epoch() -
			std::chrono::time_point_cast<step>(_last).time_since_epoch();

		Instrumentor::Get().WriteProfile(ProfileResult{_name, highResStart, elapsedTime, std::this_thread::get_id()});

		_stopped = true;
	}
	catch (...)
	{
		std::cerr << "Failed to stop instrumentor timer" << std::endl;
	}
#endif
}
