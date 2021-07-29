#include "SaffronPCH.h"

#include "Saffron/Rendering/Bindable.h"

namespace Se
{
void Bindable::Initialize()
{
	if (_initializer.has_value())
	{
		(*_initializer)();
	}
}

void Bindable::SetInitializer(std::function<void()> initializer)
{
	_initializer = initializer;
}
}
