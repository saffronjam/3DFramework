#pragma once

#include <Saffron.h>

class SandboxLayer : public Se::Layer
{
public:
	SandboxLayer(Se::Window &window, const Se::Keyboard &keyboard, const Se::Mouse &mouse);

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(Se::Time ts) override;
	void OnImGuiRender() override;

	void OnEvent(const Se::Event &event) override;

private:
	Se::Window &m_Window;
	const Se::Keyboard &m_Keyboard;
	const Se::Mouse &m_Mouse;

	Se::ShaderLibrary m_ShaderLibrary;

	Se::Ref<Se::VertexArray> m_SquareVA;

	Se::Ref<Se::Texture2D> m_Texture;

	Se::CameraController2D m_CameraController2D;
	Se::CameraControllerEditor m_CameraControllerEditor;

	bool m_EditorModeOn = false;
};
