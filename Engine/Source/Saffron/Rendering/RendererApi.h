#pragma once

#include "Saffron/Math/Math.h"

namespace Se
{
class RendererApi
{
public:
	static void Clear(const Vector4& color = { 0.0f, 0.0f, 0.0f, 1.0f });
	static void Present();
};
}
