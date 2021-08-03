#pragma once

#include <string>

#include "Saffron/Base.h"
#include "Saffron/Event/Event.h"
#include "Saffron/Rendering/Image.h"
#include "Saffron/Ui/Ui.h"

namespace Se
{
class ViewportPanel
{
public:
	explicit ViewportPanel(const std::string& title, const std::weak_ptr<Image>& image);
	explicit ViewportPanel(const std::string& title);
	explicit ViewportPanel();

	void OnUi();

	void SetImage(const std::shared_ptr<Image>& image);

	auto MousePosition(bool normalized = false) const -> Vector2;

public:
	SubscriberList<const SizeEvent&> Resized;

private:
	std::weak_ptr<Image> _image;
	std::shared_ptr<Image> _fallback;
	std::string _title;

	Vector2 _topLeft = {0.0f, 0.0f};
	Vector2 _bottomRight = {0.0f, 0.0f};
	Vector2 _viewportSize = {0.0f, 0.0f};
};
}
