#pragma once

#include "Saffron/Core/Timer.h"
#include "Saffron/Core/TypeDefs.h"

namespace Se
{
struct ProfileResult
{
	String Name;

	Timer::TimePoint Start;
	Time ElapsedTime;
	Thread::id ThreadID;
};

struct InstrumentationSession
{
	String Name;
};

class Instrumentor
{
public:
	Instrumentor(const Instrumentor &) = delete;
	Instrumentor(Instrumentor &&) = delete;

	void BeginSession(const String &name, const String &filepath = "results.json");
	void EndSession();

	void WriteProfile(const ProfileResult &result);

	static Instrumentor &Get();
private:
	Instrumentor();
	~Instrumentor();

	void WriteHeader();
	void WriteFooter();

	// Note: you must already own lock on m_Mutex before
	// calling InternalEndSession()
	void InternalEndSession();

private:
	Mutex m_Mutex;
	InstrumentationSession *m_CurrentSession;
	OutputStream m_OutputStream;
};

class InstrumentationTimer : public Timer
{
public:
	explicit InstrumentationTimer(const char *name);
	~InstrumentationTimer();

	void Stop();

private:
	bool m_Stopped;
};

namespace InstrumentorUtils
{
template <size_t N>
struct ChangeResult
{
	char Data[N];
};

template <size_t N, size_t K>
constexpr auto CleanupOutputString(const char(&expr)[N], const char(&remove)[K])
{
	ChangeResult<N> result = {};

	size_t srcIndex = 0;
	size_t dstIndex = 0;
	while ( srcIndex < N )
	{
		size_t matchIndex = 0;
		while ( matchIndex < K - 1 && srcIndex + matchIndex < N - 1 && expr[srcIndex + matchIndex] == remove[matchIndex] )
			matchIndex++;
		if ( matchIndex == K - 1 )
			srcIndex += matchIndex;
		result.Data[dstIndex++] = expr[srcIndex] == '"' ? '\'' : expr[srcIndex];
		srcIndex++;
	}
	return result;
}
}
}

#define SE_PROFILE 1
#if SE_PROFILE
// Resolve which function signature macro will be used. Note that this only
// is resolved when the (pre)compiler starts, so the syntax highlighting
// could mark the wrong one in your editor!
#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
#define SE_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
#define SE_FUNC_SIG __PRETTY_FUNCTION__
#elif (defined(__FUNCSIG__) || (_MSC_VER))
#define SE_FUNC_SIG __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
#define SE_FUNC_SIG __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
#define SE_FUNC_SIG __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#define SE_FUNC_SIG __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
#define SE_FUNC_SIG __func__
#else
#define SE_FUNC_SIG "SE_FUNC_SIG unknown!"
#endif

#define SE_PROFILE_BEGIN_SESSION(name, filepath) ::Se::Instrumentor::Instance().BeginSession(name, filepath)
#define SE_PROFILE_END_SESSION() ::Se::Instrumentor::Instance().EndSession()
#define SE_PROFILE_SCOPE_LINE2(name, line) constexpr auto fixedName##line = ::Se::InstrumentorUtils::CleanupOutputString(name, "__cdecl ");\
											   ::Se::InstrumentationTimer timer##line(fixedName##line.Data)
#define SE_PROFILE_SCOPE_LINE(name, line) SE_PROFILE_SCOPE_LINE2(name, line)
#define SE_PROFILE_SCOPE(name) SE_PROFILE_SCOPE_LINE(name, __LINE__)
#define SE_PROFILE_FUNCTION() SE_PROFILE_SCOPE(SE_FUNC_SIG)
#else
#define SE_PROFILE_BEGIN_SESSION(name, filepath)
#define SE_PROFILE_END_SESSION()
#define SE_PROFILE_SCOPE(name)
#define SE_PROFILE_FUNCTION()
#endif
