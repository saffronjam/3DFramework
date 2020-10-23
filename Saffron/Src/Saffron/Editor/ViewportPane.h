#pragma once

#include "Saffron/Core/Math/SaffronMath.h"

namespace Se
{
class ViewportPane
{
public:
	explicit ViewportPane(std::string renderTargetIdentifier);

	void OnGuiRender();

	bool InViewport(glm::vec2 positionNDC) const;

	glm::vec2 GetMousePosition() const;
	glm::vec2 GetViewportSize() const;
	const glm::vec2 &GetTopLeft() const { return m_TopLeft; }
	const glm::vec2 &GetBottomRight() const { return m_BottomRight; }
	bool IsHovered() const { return m_Hovered; }
	bool IsFocused() const { return m_Focused; }

	void SetPostRenderCallback(std::function<void()> fn) { m_PostRenderFunction = fn; }
	void SetRenderTargetIdentifier(std::string renderTargetIdentifier) { m_RenderTargetIdentifier = std::move(renderTargetIdentifier); }

private:
	std::string m_RenderTargetIdentifier;

	glm::vec2 m_TopLeft;
	glm::vec2 m_BottomRight;
	bool m_Hovered;
	bool m_Focused;
	std::function<void()> m_PostRenderFunction;

};

}