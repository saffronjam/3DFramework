#pragma once

#include "Saffron/Base.h"

namespace Se
{
class BatchLoader : public Managed
{
public:
	explicit BatchLoader(String name);
	~BatchLoader();

	void Submit(Function<void()> function, String shortDescription);
	void Execute();
	void ForceExit();
	void Reset();

	float GetProgress() const { return _progress; }

	const String* GetStatus() const { return _status; }

	bool IsFinished() const { return _progress >= 100.0f; }

	Mutex& GetExecutionMutex() { return _executionMutex; }

public:
	mutable EventSubscriberList<void> Started;
	mutable EventSubscriberList<void> Finished;

private:
	String _name;

	ArrayList<Pair<Function<void()>, String>> _queue;
	Atomic<float> _progress = 0.0f;
	Atomic<const String*> _status = nullptr;
	Atomic<bool> _running = false, _shouldExit = false;
	Mutex _queueMutex, _executionMutex;
	Thread _worker;
};
}
