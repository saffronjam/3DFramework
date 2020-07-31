#pragma once

#include "Saffron/Config.h"
#include "Saffron/Renderer/RendererAPI.h"

namespace Se
{
class OpenGLRendererAPI : public RendererAPI
{
public:
	void Init() override;
	void SetViewport(Uint32 x, Uint32 y, Uint32 width, Uint32 height) override;

	void SetClearColor(const glm::vec4 &color) override;
	void Clear() override;

	void DrawIndexed(const Ref<VertexArray> &vertexArray, Uint32 indexCount = 0) override;
};
}
