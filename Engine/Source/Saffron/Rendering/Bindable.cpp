#include "SaffronPCH.h"

#include "Saffron/Rendering/Bindable.h"

namespace Se
{
void Bindable::Initialize()
{
	if (_initializer.has_value())
	{
		for (auto& initializer : *_initializer)
		{
			initializer();
		}
	}
}

void Bindable::SetInitializer(std::function<void()> initializer)
{
	if (!_initializer.has_value())
	{
		_initializer = std::make_optional<std::vector<std::function<void()>>>();
	}
	_initializer->push_back(initializer);
}
}
