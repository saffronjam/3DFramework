#pragma once

#include "Saffron/Math/SaffronMath.h"
#include "Saffron/Rendering/SceneRenderer.h"

namespace Se
{
class ViewportPane : public Managed
{
public:
	explicit ViewportPane(String windowTitle);

	void OnGuiRender(bool* open = nullptr, UUID uuid = 0);

	bool InViewport(Vector2f positionNDC) const;

	Vector2f GetMousePosition() const;
	Vector2f GetViewportSize() const;

	Uint32 GetDockID() const { return _dockID; }

	const Vector2f& GetTopLeft() const { return _topLeft; }

	const Vector2f& GetBottomRight() const { return _bottomRight; }

	bool IsHovered() const { return _hovered; }

	bool IsFocused() const { return _focused; }

public:
	mutable EventSubscriberList<void> FinishedRender;

private:
	String _windowTitle;
	Shared<Texture> _fallbackTexture;
	Uint32 _dockID = 0;

	Vector2f _topLeft;
	Vector2f _bottomRight;
	bool _hovered;
	bool _focused;
};
}
