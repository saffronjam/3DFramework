#pragma once

#include <dxgidebug.h>

#include "Saffron/Base.h"

namespace Se
{
class DxgiInfoManager : public SingleTon<DxgiInfoManager>
{
public:
	DxgiInfoManager();

	void Begin();
	auto End() -> std::vector<std::string>;

private:
	ComPtr<IDXGIInfoQueue> _queue{};
	uint _startIndex = 0;
};
}
