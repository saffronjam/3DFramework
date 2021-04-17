#include "SaffronPCH.h"

#include "Saffron/Debug/Instrumentor.h"

namespace Se
{
void Instrumentor::BeginSession(const String& name, const String& filepath)
{
	std::lock_guard lock(_mutex);
	if (_currentSession)
	{
		// If there is already a current session, then close it before beginning new one.
		// Subsequent profiling output meant for the original session will end up in the
		// newly opened session instead.  That's better than having badly formatted
		// profiling output.
		if (Log::GetCoreLogger()) // Edge case: BeginSession() might be before Log::Init()
		{
			SE_CORE_ERROR("Instrumentor::BeginSession('{0}') when session '{1}' already open.", name,
			              _currentSession->Name);
		}
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
		if (Log::GetCoreLogger()) // Edge case: BeginSession() might be before Log::Init()
		{
			SE_CORE_ERROR("Instrumentor could not open results file '{0}'.", filepath);
		}
	}
}

void Instrumentor::EndSession()
{
	std::lock_guard lock(_mutex);
	InternalEndSession();
}

void Instrumentor::WriteProfile(const ProfileResult& result)
{
	StringStream json;

	json << std::setprecision(3) << std::fixed;
	json << R"(,{)";
	json << R"("cat":"function",)";
	json << R"("dur":)" << result.ElapsedTime.us() << R"(,)";
	json << R"("name":")" << result.Name << R"(",)";
	json << R"("ph":"X",)";
	json << R"("pid":0,)";
	json << R"("tid":)" << result.ThreadID << ",";
	json << R"("ts":)" << result.Start.time_since_epoch().count();
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

InstrumentationTimer::InstrumentationTimer(const char* name) :
	Timer(name),
	_stopped(false)
{
}

InstrumentationTimer::~InstrumentationTimer()
{
	if (!_stopped) Stop();
}

void InstrumentationTimer::Stop()
{
	try
	{
		Instrumentor::Get().WriteProfile(ProfileResult{_name, GetStart(), Peek(), std::this_thread::get_id()});

		_stopped = true;
	}
	catch (...)
	{
		SE_WARN("Failed to stop instrumentor timer");
	}
}
}
