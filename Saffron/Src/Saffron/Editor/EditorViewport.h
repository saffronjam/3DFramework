#pragma once

#include "Saffron/Core/Math/SaffronMath.h"

namespace Se
{
class EditorViewport
{
public:
	static void OnUpdate();
	static void OnGuiRender();

	static bool InViewport(glm::vec2 positionNDC);

	static glm::vec2 GetMousePosition();
	static glm::vec2 GetViewportSize();
	static const glm::vec2 &GetTopLeft() { return m_TopLeft; }
	static const glm::vec2 &GetBottomRight() { return m_BottomRight; }
	static bool IsHovered() { return m_Hovered; }
	static bool IsFocused() { return m_Focused; }

	static void SetPostRenderCallback(std::function<void()> fn) { m_PostRenderFunction = fn; }

private:
	static glm::vec2 m_TopLeft;
	static glm::vec2 m_BottomRight;
	static bool m_Hovered;
	static bool m_Focused;
	static std::function<void()> m_PostRenderFunction;

};

}