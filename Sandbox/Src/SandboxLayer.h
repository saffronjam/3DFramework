#pragma once

#include <Saffron.h>

class SandboxLayer : public Se::Layer
{
public:
	SandboxLayer(const Se::Keyboard &keyboard, const Se::Mouse &mouse);

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(Se::Time ts) override;
	void OnImGuiRender() override;

	void OnEvent(const Se::Event &event) override;

private:
	const Se::Keyboard &m_Keyboard;
	const Se::Mouse &m_Mouse;

	Se::ShaderLibrary m_ShaderLibrary;

	Se::Ref<Se::VertexArray> m_SquareVA;

	Se::Ref<Se::Texture2D> m_Texture;

	Se::CameraController2D m_CameraController2D;
	Se::CameraController3D m_CameraController3D;

	bool m_PerspectiveOn = false;
};
