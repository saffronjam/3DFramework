#include "SaffronPCH.h"

#include <comdef.h>

#include "Saffron/Rendering/ErrorHandling/HrException.h"

namespace Se
{
auto HrException::What() const -> std::string
{
	return Formatted<HrException>();
}

auto HrException::Type() const -> const char*
{
	return "HResult Exception";
}

auto HrException::HResultMessage(HRESULT hResult, char buffer[]) -> const char*
{
	const _com_error err(hResult);
	const auto* errMsg = err.ErrorMessage();
	sprintf(buffer, "%ls", errMsg);
	return buffer;
}
}
