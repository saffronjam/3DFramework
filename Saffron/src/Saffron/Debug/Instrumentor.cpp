#include "SaffronPCH.h"

#include "Saffron/Debug/Instrumentor.h"

namespace Se
{

void Instrumentor::BeginSession(const String &name, const String &filepath)
{
	std::lock_guard lock(m_Mutex);
	if ( m_CurrentSession )
	{
		// If there is already a current session, then close it before beginning new one.
		// Subsequent profiling output meant for the original session will end up in the
		// newly opened session instead.  That's better than having badly formatted
		// profiling output.
		if ( Log::GetCoreLogger() ) // Edge case: BeginSession() might be before Log::Init()
		{
			SE_CORE_ERROR("Instrumentor::BeginSession('{0}') when session '{1}' already open.", name, m_CurrentSession->Name);
		}
		InternalEndSession();
	}
	m_OutputStream.open(filepath);

	if ( m_OutputStream.is_open() )
	{
		m_CurrentSession = new InstrumentationSession({ name });
		WriteHeader();
	}
	else
	{
		if ( Log::GetCoreLogger() ) // Edge case: BeginSession() might be before Log::Init()
		{
			SE_CORE_ERROR("Instrumentor could not open results file '{0}'.", filepath);
		}
	}
}

void Instrumentor::EndSession()
{
	std::lock_guard lock(m_Mutex);
	InternalEndSession();
}

void Instrumentor::WriteProfile(const ProfileResult &result)
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

	std::lock_guard lock(m_Mutex);
	if ( m_CurrentSession )
	{
		m_OutputStream << json.str();
		m_OutputStream.flush();
	}
}

Instrumentor &Instrumentor::Get()
{
	static Instrumentor instance;
	return instance;
}

Instrumentor::Instrumentor() : m_CurrentSession(nullptr)
{
}

Instrumentor::~Instrumentor()
{
	EndSession();
}

void Instrumentor::WriteHeader()
{
	m_OutputStream << R"({"otherData": {},"traceEvents":[{})";
	m_OutputStream.flush();
}

void Instrumentor::WriteFooter()
{
	m_OutputStream << "]}";
	m_OutputStream.flush();
}

void Instrumentor::InternalEndSession()
{
	if ( m_CurrentSession )
	{
		WriteFooter();
		m_OutputStream.close();
		delete m_CurrentSession;
		m_CurrentSession = nullptr;
	}
}

InstrumentationTimer::InstrumentationTimer(const char *name)
	:
	Timer(name),
	m_Stopped(false)
{
}

InstrumentationTimer::~InstrumentationTimer()
{
	if ( !m_Stopped )
		Stop();
}

void InstrumentationTimer::Stop()
{
	try
	{
		Instrumentor::Get().WriteProfile(ProfileResult{ m_Name, GetStart(), Peek(), std::this_thread::get_id() });

		m_Stopped = true;
	}
	catch ( ... )
	{
		SE_WARN("Failed to stop instrumentor timer");
	}
}
}
