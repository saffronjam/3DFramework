#pragma once

//TODO: Remove
#include <memory>

#include "Saffron/Config.h"
#include "Saffron/Core/Assert.h"
#include "Saffron/Core/Log.h"
#include "Saffron/Core/SmartPointers.h"
#include "Saffron/System/Macros.h"
#include "Saffron/System/TypeDefs.h"


namespace Se
{

// TODO: Implement this as a class
/////////////////////////////////////////////////////////
/// Smart pointer abstraction
/////////////////////////////////////////////////////////

template<typename T>
using Scope = std::unique_ptr<T>;
template<typename T, typename ... Args>
constexpr Scope<T> CreateScope(Args && ... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

}
