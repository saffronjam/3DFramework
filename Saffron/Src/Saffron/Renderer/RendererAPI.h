#pragma once

#include "Saffron/Config.h"
#include "Saffron/Renderer/VertexArray.h"
#include "Saffron/System/SaffronMath.h"

namespace Se
{
class RendererAPI
{
public:
	enum class API
	{
		None = 0, OpenGL = 1
	};

public:
	virtual ~RendererAPI() = default;

	virtual void Init() = 0;

	virtual void SetViewport(Uint32 x, Uint32 y, Uint32 width, Uint32 height) = 0;
	virtual void SetClearColor(const glm::vec4 &color) = 0;
	virtual void Clear() = 0;

	virtual void DrawIndexed(const Ref<VertexArray> &vertexArray, Uint32 indexCount = 0) = 0;

	static API GetAPI();
	static Scope<RendererAPI> Create();

private:
	static API m_sAPI;
};
}
