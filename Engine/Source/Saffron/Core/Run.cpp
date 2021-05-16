#include "SaffronPCH.h"

#include "Saffron/Core/Global.h"
#include "Saffron/Core/Run.h"

namespace Se
{
List<Function<void()>> Run::_laterFunctions;
TreeMap<Run::Handle, Run::PeriodicFunction> Run::_periodicFunctions;

void Run::Execute()
{
	const auto ts = Global::Timer::GetStep();

	for (auto& function : _laterFunctions)
	{
		function();
	}
	_laterFunctions.clear();

	for (auto& [handle, periodcalFunction] : _periodicFunctions)
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
	_laterFunctions.push_back(function);
}

Run::Handle Run::Periodically(const Function<void()>& function, Time interval)
{
	Handle newHandle;
	_periodicFunctions.emplace(newHandle, PeriodicFunction{function, interval, Time::Zero()});
	return newHandle;
}

void Run::Remove(Handle handle)
{
	_periodicFunctions.erase(handle);
}
}
