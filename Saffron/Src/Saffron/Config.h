#pragma once

// TODO: Remove once unique_ptr is in own class
#include <memory>

#include <Saffron/Core/Assert.h>
#include <Saffron/Core/Ref.h>
#include "Saffron/System/Platform.h"
#include "Saffron/System/TypeDefs.h"


#ifndef SE_PLATFORM_WINDOWS
#error Saffron only support Windows!
#endif


namespace Se
{

// TODO: Implement this as a class
/////////////////////////////////
/// Smart pointer abstraction ///
/////////////////////////////////

template<typename T>
using Scope = std::unique_ptr<T>;
template<typename T, typename ... Args>
constexpr Scope<T> CreateScope(Args && ... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

}
