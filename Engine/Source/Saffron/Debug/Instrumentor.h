#pragma once

#include <string>
#include <chrono>
#include <mutex>
#include <thread>
#include <ostream>
#include <fstream>

#define ENABLE_PROFILE
#ifdef ENABLE_PROFILE
// Resolve which function signature macro will be used
#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
#define PF_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
#define PF_FUNC_SIG __PRETTY_FUNCTION__
#elif (defined(__FUNCSIG__) || (_MSC_VER))
#define PF_FUNC_SIG __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
#define PF_FUNC_SIG __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
#define PF_FUNC_SIG __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#define PF_FUNC_SIG __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
#define PF_FUNC_SIG __func__
#else
#define PF_FUNC_SIG "PF_FUNC_SIG unknown!"
#endif
#else
#define PF_FUNC_SIG ""
#endif


struct ProfileResult
{
	using Duration = std::chrono::duration<double, std::nano>;

	std::string Name;

	Duration Start;
	std::chrono::nanoseconds ElapsedTime;
	std::thread::id ThreadID;
};

struct InstrumentationSession
{
	std::string Name;
};

class Instrumentor
{
public:
	Instrumentor(const Instrumentor&) = delete;
	Instrumentor(Instrumentor&&) = delete;

	void BeginSession(const std::string& name, const std::string& filepath = "results.json");
	void EndSession();

	void WriteProfile(const ProfileResult& result);

	static Instrumentor& Get();
private:
	Instrumentor();
	~Instrumentor();

	void WriteHeader();
	void WriteFooter();

	// Note: you must already own lock on _mutex before
	// calling InternalEndSession()
	void InternalEndSession();

private:
	std::mutex _mutex;
	InstrumentationSession* _currentSession;
	std::ofstream _outputStream;
};


class InstrumentationTimer
{
	using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

public:
	explicit InstrumentationTimer(const char* name);
	~InstrumentationTimer();

	void Stop();

private:
	const char* _name;
	TimePoint _last;
	bool _stopped;
};

class Profiler
{
public:
	static void BeginSession(const std::string& name, const std::string& filepath = "results.json");
	static void EndSession();

	// Use PF_FUNC_SIG to generate function name automatically
	static void Function(const char* functionName);
	static InstrumentationTimer Scope(const char* name);
};
