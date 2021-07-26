#pragma once

#include "Saffron/ErrorHandling/SaffronException.h"

namespace Se
{
using HRESULT = long;

class HrException : public SaffronException
{
public:
	HrException(HRESULT hResult, const std::source_location& location) :
		SaffronException(HResultMessage(hResult, _hResultMessageBuffer), location),
		_hResult(hResult)
	{
	}

	auto What() const -> std::string override
	{
		return Formatted<HrException>();
	}

	auto Type() const -> const char* override;

private:
	static auto HResultMessage(HRESULT hResult, char buffer[]) -> const char*;

private:
	HRESULT _hResult;
	char _hResultMessageBuffer[256] = {};
	std::string _messageBuffer;
};

constexpr bool GoodHResult(long hResult)
{
	return hResult >= 0;
}

constexpr bool BadHResult(long hResult)
{
	return !GoodHResult(hResult);
}
}
