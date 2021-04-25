#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/Resources/Texture.h"

namespace Se
{
class ViewportPane : public Managed
{
public:
	explicit ViewportPane(String windowTitle);

	void OnGuiRender(bool* open = nullptr, UUID uuid = 0);

	bool InViewport(Vector2 positionNDC) const;

	Vector2 GetMousePosition() const;
	Vector2 GetViewportSize() const;

	uint GetDockID() const;

	const Vector2& GetTopLeft() const;
	const Vector2& GetBottomRight() const;

	bool IsHovered() const;
	bool IsFocused() const;

public:
	mutable EventSubscriberList<void> FinishedRender;

private:
	String _windowTitle;
	Shared<Texture> _fallbackTexture;
	uint _dockID = 0;

	Vector2 _topLeft;
	Vector2 _bottomRight;
	bool _hovered;
	bool _focused;

	uint _oldWidth = 0, _oldHeight = 0;
};
}
