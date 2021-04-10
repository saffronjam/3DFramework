#include "SaffronPCH.h"

#include "Saffron/Common/Core.h"

namespace Se
{
bool Core::Initialize()
{
	Log::Init();


	const auto hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		return false;
	}
	return true;
}

bool Core::Shutdown()
{
	CoUninitialize();
	return true;
}
}
