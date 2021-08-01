#pragma once

#include "Saffron/Common/TimeSpan.h"

namespace Se
{
class Layer
{
public:
	virtual ~Layer() = default;
	virtual void OnAttach() = 0;
	virtual void OnDetach() = 0;
	virtual void OnUpdate(TimeSpan dt) = 0;
	virtual void OnUi() = 0;
};
}
