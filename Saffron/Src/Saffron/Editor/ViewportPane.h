#pragma once

#include "Saffron/Core/Math/SaffronMath.h"
#include "Saffron/Renderer/SceneRenderer.h"

namespace Se
{
class ViewportPane : public ReferenceCounted
{
public:
	explicit ViewportPane(String windowTitle, Shared<SceneRenderer::Target> target);

	void OnGuiRender(bool *open = nullptr);

	bool InViewport(Vector2f positionNDC) const;

	Vector2f GetMousePosition() const;
	Vector2f GetViewportSize() const;
	const Vector2f &GetTopLeft() const { return m_TopLeft; }
	const Vector2f &GetBottomRight() const { return m_BottomRight; }
	bool IsHovered() const { return m_Hovered; }
	bool IsFocused() const { return m_Focused; }

	void SetPostRenderCallback(Function<void()> fn) { m_PostRenderFunction = fn; }
	void SetTarget(Shared<SceneRenderer::Target> target) { m_Target = Move(target); }

private:
	String m_WindowTitle;
	Shared<SceneRenderer::Target> m_Target;

	Vector2f m_TopLeft;
	Vector2f m_BottomRight;
	bool m_Hovered;
	bool m_Focused;
	Function<void()> m_PostRenderFunction;

};

}