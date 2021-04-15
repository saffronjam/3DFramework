#include "SaffronPCH.h"

#include "Saffron/Core/Global.h"
#include "Saffron/Core/Run.h"

namespace Se
{
ArrayList<Function<void()>> Run::m_LaterFunctions;
Map<Run::Handle, Run::PeriodicFunction> Run::m_PeriodicFunctions;

void Run::Execute()
{
	const auto ts = Global::Timer::GetStep();

	for (auto& function : m_LaterFunctions)
	{
		function();
	}
	m_LaterFunctions.clear();

	for (auto& [handle, periodcalFunction] : m_PeriodicFunctions)
	{
		periodcalFunction.currentCounter += ts;
		if (periodcalFunction.currentCounter >= periodcalFunction.interval)
		{
			periodcalFunction.currentCounter = Time::Zero();
			periodcalFunction.function();
		}
	}
}

void Run::Later(const Function<void()>& function)
{
	m_LaterFunctions.push_back(function);
}

Run::Handle Run::Periodically(const Function<void()>& function, Time interval)
{
	Handle newHandle;
	m_PeriodicFunctions.emplace(newHandle, PeriodicFunction{function, interval, Time::Zero()});
	return newHandle;
}

void Run::Remove(Handle handle)
{
	m_PeriodicFunctions.erase(handle);
}
}
