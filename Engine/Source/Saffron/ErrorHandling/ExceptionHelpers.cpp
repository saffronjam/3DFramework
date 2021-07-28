#include "SaffronPCH.h"

#include "Saffron/ErrorHandling/ExceptionHelpers.h"
#include "Saffron/Rendering/ErrorHandling/HrException.h"
#include "Saffron/Rendering/Renderer.h"

namespace Se
{
auto ThrowIfBad(HRESULT hResult, const std::source_location location) -> void
{
	if (BadHResult(hResult))
	{
		if (hResult != DXGI_ERROR_DEVICE_REMOVED)
		{
			throw HrException(hResult, location);
		}
		throw HrException(Renderer::Device().GetDeviceRemovedReason(), location);
	}
}
}
