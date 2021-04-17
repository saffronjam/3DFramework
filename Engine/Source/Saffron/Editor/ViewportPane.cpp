#include "SaffronPCH.h"

#include "Saffron/Editor/ViewportPane.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Input/Mouse.h"
#include "Saffron/Rendering/SceneRenderer.h"

namespace Se
{
ViewportPane::ViewportPane(String windowTitle) :
	_windowTitle(Move(windowTitle)),
	//_fallbackTexture(Texture2D::Create("Resources/Assets/Editor/FallbackViewportPaneTexture.png")),
	_topLeft(0.0f, 0.0f),
	_bottomRight(100.0f, 100.0f),
	_hovered(false),
	_focused(false)
{
}

void ViewportPane::OnGuiRender(bool* open, UUID uuid)
{
	const auto& tl = GetTopLeft();
	const auto& br = GetBottomRight();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	OutputStringStream oss;
	oss << _windowTitle << "##" << uuid;

	ImGui::Begin(oss.str().c_str(), open, ImGuiWindowFlags_NoFocusOnAppearing);

	_dockID = ImGui::GetWindowDockID();

	if (ImGui::IsWindowDocked())
	{
		auto* wnd = ImGui::FindWindowByName(_windowTitle.c_str());
		if (wnd)
		{
			ImGuiDockNode* node = wnd->DockNode;
			if (node && !node->IsHiddenTabBar())
			{
				node->WantHiddenTabBarToggle = true;
			}
		}
	}

	_hovered = ImGui::IsWindowHovered();
	_focused = ImGui::IsWindowFocused();

	const auto viewportOffset = ImGui::GetCursorPos(); // includes tab bar
	ImVec2 minBound = ImGui::GetWindowPos();
	minBound.x += viewportOffset.x;
	minBound.y += viewportOffset.y;

	const auto windowSize = ImGui::GetWindowSize();
	const ImVec2 maxBound = {
		minBound.x + windowSize.x - viewportOffset.x, minBound.y + windowSize.y - viewportOffset.y
	};
	_topLeft = {minBound.x, minBound.y};
	_bottomRight = {maxBound.x, maxBound.y};

	const auto viewportSize = GetViewportSize();
	const auto imageRendererID = SceneRenderer::GetFinalColorBufferRendererID();
	ImGui::Image(reinterpret_cast<void*>(imageRendererID), {viewportSize.x, viewportSize.y}, {0, 1}, {1, 0});
	ImGui::GetWindowDrawList()->AddRect(ImVec2(_topLeft.x, tl.y), ImVec2(br.x, br.y),
	                                    _focused ? IM_COL32(255, 140, 0, 180) : IM_COL32(255, 140, 0, 80), 0.0f,
	                                    ImDrawCornerFlags_All, 4);

	FinishedRender.Invoke();

	ImGui::End();
	ImGui::PopStyleVar();

	SceneRenderer::SetViewportSize(static_cast<Uint32>(viewportSize.x), static_cast<Uint32>(viewportSize.y));
}

bool ViewportPane::InViewport(Vector2f positionNDC) const
{
	positionNDC.x -= _topLeft.x;
	positionNDC.y -= _topLeft.y;
	return positionNDC.x < _bottomRight.x && positionNDC.y < _bottomRight.y;
}

Vector2f ViewportPane::GetMousePosition() const
{
	Vector2f position = Mouse::GetPositionNDC();
	position.x -= _topLeft.x;
	position.y -= _topLeft.y;
	const auto viewportWidth = _bottomRight.x - _topLeft.x;
	const auto viewportHeight = _bottomRight.y - _topLeft.y;

	return {position.x / viewportWidth * 2.0f - 1.0f, (position.y / viewportHeight * 2.0f - 1.0f) * -1.0f};
}

Vector2f ViewportPane::GetViewportSize() const
{
	return GetBottomRight() - GetTopLeft();
}
}
