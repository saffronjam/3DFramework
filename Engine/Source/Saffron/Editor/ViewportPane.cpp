#include "SaffronPCH.h"

#include "Saffron/Editor/ViewportPane.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Input/Mouse.h"
#include "Saffron/Renderer/SceneRenderer.h"

namespace Se
{

ViewportPane::ViewportPane(String windowTitle, Shared<SceneRenderer::Target> target)
	: m_WindowTitle(Move(windowTitle)),
	m_Target(Move(target)),
	//m_FallbackTexture(Factory::Create<Texture2D>("Resources/Assets/Editor/FallbackViewportPaneTexture.png")),
	m_TopLeft(0.0f, 0.0f),
	m_BottomRight(100.0f, 100.0f),
	m_Hovered(false),
	m_Focused(false)
{
}

void ViewportPane::OnGuiRender(bool *open, UUID uuid)
{
	const auto &tl = GetTopLeft();
	const auto &br = GetBottomRight();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	OutputStringStream oss;
	oss << m_WindowTitle << "##" << uuid;

	ImGui::Begin(oss.str().c_str(), open, ImGuiWindowFlags_NoFocusOnAppearing);

	m_DockID = ImGui::GetWindowDockID();

	if ( ImGui::IsWindowDocked() )
	{
		auto *wnd = ImGui::FindWindowByName(m_WindowTitle.c_str());
		if ( wnd )
		{
			ImGuiDockNode *node = wnd->DockNode;
			if ( node && !node->IsHiddenTabBar() )
			{
				node->WantHiddenTabBarToggle = true;
			}
		}
	}

	m_Hovered = ImGui::IsWindowHovered();
	m_Focused = ImGui::IsWindowFocused();

	const auto viewportOffset = ImGui::GetCursorPos(); // includes tab bar
	ImVec2 minBound = ImGui::GetWindowPos();
	minBound.x += viewportOffset.x;
	minBound.y += viewportOffset.y;

	const auto windowSize = ImGui::GetWindowSize();
	const ImVec2 maxBound = { minBound.x + windowSize.x - viewportOffset.x, minBound.y + windowSize.y - viewportOffset.y };
	m_TopLeft = { minBound.x, minBound.y };
	m_BottomRight = { maxBound.x, maxBound.y };

	const auto viewportSize = GetViewportSize();
	const auto imageRendererID = m_Target->IsEnabled() ? m_Target->GetFinalColorBufferRendererID() : m_FallbackTexture->GetRendererID();
	ImGui::Image(reinterpret_cast<void *>(imageRendererID), { viewportSize.x, viewportSize.y }, { 0, 1 }, { 1, 0 });
	ImGui::GetWindowDrawList()->AddRect(ImVec2(m_TopLeft.x, tl.y), ImVec2(br.x, br.y), m_Focused ? IM_COL32(255, 140, 0, 180) : IM_COL32(255, 140, 0, 80), 0.0f, ImDrawCornerFlags_All, 4);

	FinishedRender.Invoke();

	ImGui::End();
	ImGui::PopStyleVar();

	m_Target->SetSize(static_cast<Uint32>(viewportSize.x), static_cast<Uint32>(viewportSize.y));
}

bool ViewportPane::InViewport(Vector2f positionNDC) const
{
	positionNDC.x -= m_TopLeft.x;
	positionNDC.y -= m_TopLeft.y;
	return positionNDC.x < m_BottomRight.x &&positionNDC.y < m_BottomRight.y;
}

Vector2f ViewportPane::GetMousePosition() const
{
	Vector2f position = Mouse::GetPositionNDC();
	position.x -= m_TopLeft.x;
	position.y -= m_TopLeft.y;
	const auto viewportWidth = m_BottomRight.x - m_TopLeft.x;
	const auto viewportHeight = m_BottomRight.y - m_TopLeft.y;

	return { position.x / viewportWidth * 2.0f - 1.0f, (position.y / viewportHeight * 2.0f - 1.0f) * -1.0f };
}

Vector2f ViewportPane::GetViewportSize() const
{
	return GetBottomRight() - GetTopLeft();
}
}
