#pragma once

#include "Saffron/Base.h"

namespace Se
{
class BatchLoader : public Managed
{
public:
	explicit BatchLoader(String name);
	virtual ~BatchLoader();

	SE_FUNCTION();

	void MyTestFunction(int parameter1, float parameter2)
	{
	}

	SE_FUNCTION();

	void MyTestFunction2(double parameter1, char parameter2)
	{
	}

	class MySubClass
	{
		SE_FUNCTION();
		auto   eyo  (  int   testing   )  ->  int;

		class MySecondSubClass
		{
			SE_FUNCTION();
			  auto eyo   (  int   testing   )  ->  int   ;
		};
	};


	void Submit(Function<void()> function, String shortDescription);
	void Execute();
	void ForceExit();
	void Reset();

	auto GetProgress() const -> float;
	auto GetStatus() const -> const String*;

	auto IsFinished() const -> bool;

	auto GetExecutionMutex() -> Mutex&;

public:
	mutable EventSubscriberList<void> Started;
	mutable EventSubscriberList<void> Finished;

private:
	String _name;

	List<Pair<Function<void()>, String>> _queue;
	Atomic<float> _progress = 0.0f;
	Atomic<const String*> _status = nullptr;
	Atomic<bool> _running = false, _shouldExit = false;
	Mutex _queueMutex, _executionMutex;
	Thread _worker;
};
}
