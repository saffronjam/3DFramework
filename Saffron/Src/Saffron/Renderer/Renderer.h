#pragma once

#include "Saffron/Event/WindowEvent.h"
#include "Saffron/Renderer/Camera2D.h"
#include "Saffron/Renderer/RenderCommand.h"
#include "Saffron/Renderer/RendererAPI.h"
#include "Saffron/Renderer/Shader.h"
#include "Saffron/System/SaffronMath.h"

namespace Se
{
class Renderer
{
public:
	virtual ~Renderer() = default;

	static void Init();
	static void Shutdown();

	static void OnEvent(const Event &event);

	static void BeginScene(Camera2D &camera);
	static void EndScene();

	static void Submit(const Ref<Shader> &shader, const Ref<VertexArray> &vertexArray, const glm::mat4 &transform = glm::mat4(1.0f));

	static RendererAPI::API GetAPI();

private:
	static void OnWindowResize(const WindowResizeEvent &event);

private:
	struct SceneData
	{
		glm::mat4 ViewProjectionMatrix;
	};

	static Scope<SceneData> m_sSceneData;
};
}
