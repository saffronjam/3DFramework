#include "SaffronPCH.h"

#include "Saffron/Ui/ViewportPanel.h"

namespace Se
{
static int counter = 0;

ViewportPanel::ViewportPanel(const std::string& title, const std::weak_ptr<Image>& image) :
	_title(title + "##Viewport#" + std::to_string(counter++)),
	_fallback(Image::Create(ImageSpec{100, 100, ImageUsage_ShaderResource, ImageFormat::RGBA}))
{
}

ViewportPanel::ViewportPanel(const std::string& title) :
	_title(std::move(title)),
	_fallback(Image::Create(ImageSpec{100, 100, ImageUsage_ShaderResource, ImageFormat::RGBA}))
{
}

void ViewportPanel::OnUi()
{
	Image& toUse = _image.expired() ? *_fallback : *_image.lock();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	ImGui::Begin(_title.c_str());

	const auto viewportOffset = ImGui::GetCursorPos(); // Include tab bar
	ImVec2 minBound = ImGui::GetWindowPos();
	minBound.x += viewportOffset.x;
	minBound.y += viewportOffset.y;

	const auto windowSize = ImGui::GetWindowSize();
	const ImVec2 maxBound = {
		minBound.x + windowSize.x - viewportOffset.x, minBound.y + windowSize.y - viewportOffset.y
	};
	_topLeft = Vector2{minBound.x, minBound.y};
	_bottomRight = Vector2{maxBound.x, maxBound.y};
	const auto viewportSize = _bottomRight - _topLeft;

	if (_viewportSize != viewportSize)
	{
		Resized.Invoke(SizeEvent(viewportSize.x, viewportSize.y));
	}
	_viewportSize = viewportSize;

	ImGui::Image(&toUse.ShaderView(), {_viewportSize.x, _viewportSize.y});

	ImGui::End();
	ImGui::PopStyleVar();
}

void ViewportPanel::SetImage(const std::shared_ptr<Image>& image)
{
	_image = image;
}
}
