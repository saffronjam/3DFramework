#pragma once

#include <Saffron.h>

class SandboxLayer : public Se::Layer
{
public:
	SandboxLayer();

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(Se::Time ts) override;
	void OnImGuiRender() override;

	void OnEvent(const Se::Event &event) override;

private:
	Se::ShaderLibrary m_ShaderLibrary;
	Se::Ref<Se::Shader> m_Shader;
	Se::Ref<Se::VertexArray> m_VertexArray;

	Se::Ref<Se::Shader> m_FlatColorShader;
	Se::Ref<Se::VertexArray> m_SquareVA;

	Se::OrthographicCameraController m_CameraController;
	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
};
