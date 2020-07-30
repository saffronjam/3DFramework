#pragma once

#include "Saffron/Event/EventHandler.h"
#include "Saffron/Event/WindowEvent.h"
#include "Saffron/Graphics/Window.h"
#include "Saffron/Renderer/RenderCommand.h"
#include "Saffron/Renderer/RendererAPI.h"
#include "Saffron/Renderer/Shader.h"
#include "Saffron/System/SaffronMath.h"

namespace Se
{
class Renderer : public EventHandler
{
public:
	explicit Renderer(Window &window);
	virtual ~Renderer() = default;

	static void Init();
	static void Shutdown();

	void OnEvent(const Event &event) override;

	//static void BeginScene(OrthographicCamera &camera);
	static void EndScene();

	static void Submit(const Ref<Shader> &shader, const Ref<VertexArray> &vertexArray, const glm::mat4 &transform = glm::mat4(1.0f));

	static RendererAPI::API GetAPI();

private:
	void OnWindowResize(const WindowResizeEvent &event);

private:
	struct SceneData
	{
		glm::mat4 ViewProjectionMatrix;
	};

	static Scope<SceneData> m_sSceneData;
};
}
