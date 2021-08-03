#pragma once

#pragma warning(disable : 4244)
#pragma warning(disable : 4006)
#pragma warning(disable : 4312)
#pragma warning(disable : 4018)
#pragma warning(disable : 4267)

#include <memory>

#include "Saffron/Common/Singleton.h"
#include "Saffron/Common/SubscriberList.h"
#include "Saffron/Common/TimeSpan.h"
#include "Saffron/Core/Config.h"
#include "Saffron/Core/Macros.h"
#include "Saffron/Core/TypeDefs.h"
#include "Saffron/ErrorHandling/SaffronException.h"
#include "Saffron/Log/Log.h"
#include "Saffron/Math/Math.h"

namespace Se
{
template <typename T, typename U>
auto PtrAs(ComPtr<U>& comPtr)
{	
	ComPtr<T> result;
	const auto hr = comPtr.As(&result);
	Debug::Assert(hr >= 0);
	return result;
}

template <typename T, typename U>
auto PtrAs(const std::shared_ptr<U>& smartPointer)
{
	return std::dynamic_pointer_cast<T>(smartPointer);
}
}


#ifndef SE_FUNCTION
#define SE_FUNCTION()
#endif
