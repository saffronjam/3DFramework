#pragma once

#include <source_location>

namespace Se
{
using HRESULT = long;

auto ThrowIfBad(HRESULT hResult, const std::source_location location = std::source_location::current()) -> void;

constexpr bool GoodHResult(HRESULT hResult)
{
	return hResult >= 0;
}

constexpr bool BadHResult(HRESULT hResult)
{
	return !GoodHResult(hResult);
}
}
