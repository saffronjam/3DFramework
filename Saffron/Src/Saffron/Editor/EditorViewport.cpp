#include "SaffronPCH.h"

#include "Saffron/Editor/EditorViewport.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Input/Input.h"
#include "Saffron/Renderer/SceneRenderer.h"

namespace Se
{

glm::vec2 EditorViewport::m_TopLeft{ 0.0f, 0.0f };
glm::vec2 EditorViewport::m_BottomRight{ 0.0f, 0.0f };
bool EditorViewport::m_Hovered = false;
bool EditorViewport::m_Focused = false;
std::function<void()> EditorViewport::m_PostRenderFunction;

void EditorViewport::OnUpdate()
{

}

void EditorViewport::OnGuiRender()
{
	const auto &tl = GetTopLeft();
	const auto &br = GetBottomRight();
	if ( IsFocused() )
	{
		ImGui::GetForegroundDrawList()->AddRect(ImVec2(m_TopLeft.x, tl.y), ImVec2(br.x, br.y), IM_COL32(255, 140, 0, 180));
	}
	else
	{
		ImGui::GetForegroundDrawList()->AddRect(ImVec2(tl.x, tl.y), ImVec2(br.x, br.y), IM_COL32(255, 140, 0, 80));
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("Viewport");

	m_Hovered = ImGui::IsWindowHovered();
	m_Focused = ImGui::IsWindowFocused();

	const auto viewportOffset = ImGui::GetCursorPos(); // includes tab bar
	ImVec2 minBound = ImGui::GetWindowPos();
	minBound.x += viewportOffset.x;
	minBound.y += viewportOffset.y;

	const auto windowSize = ImGui::GetWindowSize();
	const ImVec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };
	m_TopLeft = { minBound.x, minBound.y };
	m_BottomRight = { maxBound.x, maxBound.y };

	const auto viewportSize = GetViewportSize();
	ImGui::Image(reinterpret_cast<void *>(SceneRenderer::GetFinalColorBufferRendererID()), { viewportSize.x, viewportSize.y }, { 0, 1 }, { 1, 0 });

	m_PostRenderFunction();

	ImGui::End();
	ImGui::PopStyleVar();
}

bool EditorViewport::InViewport(glm::vec2 positionNDC)
{
	positionNDC.x -= m_TopLeft.x;
	positionNDC.y -= m_TopLeft.y;
	return positionNDC.x < m_BottomRight.x &&positionNDC.y < m_BottomRight.y;
}

glm::vec2 EditorViewport::GetMousePosition()
{
	glm::vec2 position = Input::GetMousePositionNDC();
	position.x -= m_TopLeft.x;
	position.y -= m_TopLeft.y;
	const auto viewportWidth = m_BottomRight.x - m_TopLeft.x;
	const auto viewportHeight = m_BottomRight.y - m_TopLeft.y;

	return { (position.x / viewportWidth) * 2.0f - 1.0f, ((position.y / viewportHeight) * 2.0f - 1.0f) * -1.0f };
}

glm::vec2 EditorViewport::GetViewportSize()
{
	return GetBottomRight() - GetTopLeft();
}
}
