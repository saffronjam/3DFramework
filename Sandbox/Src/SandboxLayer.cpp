#include "SandboxLayer.h"

SandboxLayer::SandboxLayer(Se::Window &window, const Se::Keyboard &keyboard, const Se::Mouse &mouse)
	:
	Layer("Sandbox"),
	m_Window(window),
	m_Keyboard(keyboard),
	m_Mouse(mouse),
	m_CameraController2D(16.0f / 9.0f, true),
	m_CameraControllerEditor(16.0f / 9.0f)
{
	m_Texture = Se::Texture2D::Create("Assets/Textures/sample_image.png");
}

void SandboxLayer::OnAttach()
{
}

void SandboxLayer::OnDetach()
{
}

void SandboxLayer::OnUpdate(Se::Time ts)
{
	// Update
	if ( m_Keyboard.IsPressed(SE_KEY_2) )
		m_EditorModeOn = false;
	else if ( m_Keyboard.IsPressed(SE_KEY_3) )
		m_EditorModeOn = true;

	if ( m_EditorModeOn )
		m_CameraControllerEditor.OnUpdate(m_Keyboard, m_Mouse, ts);
	else
		m_CameraController2D.OnUpdate(m_Keyboard, m_Mouse, ts);

	// Render
	Se::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	Se::RenderCommand::Clear();

	if ( m_EditorModeOn )
		Se::Renderer2D::BeginScene(m_CameraControllerEditor.GetCamera());
	else
		Se::Renderer2D::BeginScene(m_CameraController2D.GetCamera());

	auto position = m_Mouse.GetPosition();
	position.x /= m_Window.GetWidth() * 2.0f - 1.0f;
	position.y /= m_Window.GetHeight() * 2.0f - 1.0f;
	position.y *= -1.0f;

	Se::Renderer2D::DrawQuad(position, glm::vec2(2.0f, 2.0f), m_Texture);

	Se::Renderer2D::EndScene();
}

void SandboxLayer::OnImGuiRender()
{
}

void SandboxLayer::OnEvent(const Se::Event &event)
{
	if ( m_EditorModeOn )
		m_CameraControllerEditor.OnEvent(event);
	else
		m_CameraController2D.OnEvent(event);
}
