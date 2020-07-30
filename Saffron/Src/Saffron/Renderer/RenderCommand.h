#pragma once

#include "Saffron/Renderer/RendererAPI.h"
#include "Saffron/Renderer/VertexArray.h"
#include "Saffron/System/SaffronMath.h"

namespace Se
{
class RenderCommand
{
public:
	static void Init();

	static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

	static void SetClearColor(const glm::vec4 &color);

	static void Clear();

	static void DrawIndexed(const Ref<VertexArray> &vertexArray, uint32_t count = 0);
private:
	static Scope<RendererAPI> m_sRendererAPI;
};
}

