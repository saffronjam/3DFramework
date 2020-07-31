#include "SandboxLayer.h"

SandboxLayer::SandboxLayer()
	:
	Layer("Sandbox"),
	m_CameraController(16.0f / 9.0f, true)
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
}

void SandboxLayer::OnEvent(const Se::Event &event)
{
}
