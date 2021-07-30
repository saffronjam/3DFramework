#pragma once

#include <d3d11_4.h>

#include "Saffron/Base.h"
#include "Saffron/Rendering/Bindable.h"

namespace Se
{
class Viewport : public Bindable
{
public:
	Viewport(float width, float height);

	void Bind() override;

	void Resize(float width, float height);

private:
	D3D11_VIEWPORT _nativeViewport;
};
}
