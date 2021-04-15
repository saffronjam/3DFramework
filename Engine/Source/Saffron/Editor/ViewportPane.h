#pragma once

#include "Saffron/Math/SaffronMath.h"
#include "Saffron/Rendering/SceneRenderer.h"

namespace Se
{
class ViewportPane : public Managed
{
public:
	explicit ViewportPane(String windowTitle);

	void OnGuiRender(bool *open = nullptr, UUID uuid = 0);

	bool InViewport(Vector2f positionNDC) const;

	Vector2f GetMousePosition() const;
	Vector2f GetViewportSize() const;
	Uint32 GetDockID() const { return m_DockID; }

	const Vector2f &GetTopLeft() const { return m_TopLeft; }
	const Vector2f &GetBottomRight() const { return m_BottomRight; }

	bool IsHovered() const { return m_Hovered; }
	bool IsFocused() const { return m_Focused; }

public:
	mutable EventSubscriberList<void> FinishedRender;

private:
	String m_WindowTitle;
	Shared<Texture> m_FallbackTexture;
	Uint32 m_DockID = 0;

	Vector2f m_TopLeft;
	Vector2f m_BottomRight;
	bool m_Hovered;
	bool m_Focused;

};

}