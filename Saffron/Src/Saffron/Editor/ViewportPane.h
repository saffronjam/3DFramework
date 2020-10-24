#pragma once

#include "Saffron/Core/Math/SaffronMath.h"

namespace Se
{
class ViewportPane
{
public:
	explicit ViewportPane(String renderTargetIdentifier);

	void OnGuiRender();

	bool InViewport(Vector2f positionNDC) const;

	Vector2f GetMousePosition() const;
	Vector2f GetViewportSize() const;
	const Vector2f &GetTopLeft() const { return m_TopLeft; }
	const Vector2f &GetBottomRight() const { return m_BottomRight; }
	bool IsHovered() const { return m_Hovered; }
	bool IsFocused() const { return m_Focused; }

	void SetPostRenderCallback(std::function<void()> fn) { m_PostRenderFunction = fn; }
	void SetRenderTargetIdentifier(String renderTargetIdentifier) { m_RenderTargetIdentifier = std::move(renderTargetIdentifier); }

private:
	String m_RenderTargetIdentifier;

	Vector2f m_TopLeft;
	Vector2f m_BottomRight;
	bool m_Hovered;
	bool m_Focused;
	std::function<void()> m_PostRenderFunction;

};

}