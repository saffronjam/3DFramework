#pragma once

#include "Saffron/Base.h"

namespace Se
{
class BatchLoader : public ReferenceCounted
{
public:
	explicit BatchLoader(String name);
	~BatchLoader();

	void Submit(Function<void()> function, String shortDescription);
	void Execute();
	void ForceExit();

	float GetProgress() const { return m_Progress; }
	const String *GetStatus() const { return m_Status; }
	bool IsFinished() const { return m_Progress >= 100.0f; }

	Mutex &GetExecutionMutex() { return m_ExecutionMutex; }

	void SetOnEachExecutionCallback(const Function<void()> &onEachExecution) { m_OnEachExecution = onEachExecution; }
	void SetOnStartCallback(const Function<void()> &onStart) { m_OnStart = onStart; }
	void SetOnFinishCallback(const Function<void()> &onFinish) { m_OnFinish = onFinish; }

private:
	String m_Name;

	ArrayList<Pair<Function<void()>, String>> m_Queue;
	Atomic<float> m_Progress = 0.0f;
	Atomic<const String *> m_Status = nullptr;
	Atomic<bool> m_Running = false, m_ShouldExit = false;
	Mutex m_QueueMutex, m_ExecutionMutex;
	Thread m_Worker;

	Function<void()> m_OnEachExecution, m_OnStart, m_OnFinish;
};
}

