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
		bool operator<(const Handle &other) const { return m_UUID < other.m_UUID; }
		bool operator==(const Handle &other) const { return m_UUID == other.m_UUID; }
	private:
		UUID m_UUID;
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
	static void Later(const Function<void()> &function);
	static Handle Periodically(const Function<void()> &function, Time interval);
	static void Remove(Handle handle);

private:
	static ArrayList<Function<void()>> m_LaterFunctions;
	static Map<Handle, PeriodicFunction> m_PeriodicFunctions;


};
}

