#include "SandboxLayer.h"

SandboxLayer::SandboxLayer(const Se::Window::Ptr &pWindow)
	: Layer(pWindow, "Sandbox")
{
}

void SandboxLayer::OnAttach()
{
}

void SandboxLayer::OnDetach()
{
}

void SandboxLayer::OnUpdate(Se::Time ts)
{
}

void SandboxLayer::OnImGuiRender()
{
	//bool b = true;
	//if ( m_pWindow->kbd.IsDown(Se::Keyboard::Key::A) )
	//	b = false;

	//ImGui::ShowDemoWindow(&b);
}

void SandboxLayer::OnEvent(const Se::Event::Ptr &pEvent)
{
}
