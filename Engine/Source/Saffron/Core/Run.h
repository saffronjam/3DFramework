#pragma once

#include "Saffron/Core/UUID.h"
#include "Saffron/Core/Time.h"

namespace Se
{
class Run
{
public:
	struct Handle
	{
		bool operator<(const Handle& other) const { return _uuid < other._uuid; }

		bool operator==(const Handle& other) const { return _uuid == other._uuid; }

	private:
		UUID _uuid;
	};

private:
	struct PeriodicFunction
	{
		Function<void()> function;
		Time interval;
		Time currentCounter;
	};

public:
	static void Execute();
	static void Later(const Function<void()>& function);
	static Handle Periodically(const Function<void()>& function, Time interval);
	static void Remove(Handle handle);

private:
	static ArrayList<Function<void()>> _laterFunctions;
	static Map<Handle, PeriodicFunction> _periodicFunctions;
};
}
