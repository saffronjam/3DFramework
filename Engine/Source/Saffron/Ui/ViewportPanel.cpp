#include "SaffronPCH.h"

#include "Saffron/Ui/ViewportPanel.h"

#include "Saffron/Input/Mouse.h"

namespace Se
{
static int counter = 0;

ViewportPanel::ViewportPanel(const std::string& title, const std::weak_ptr<Image>& image) :
	_image(image),
	_fallback(Image::Create({100, 100, ImageUsage_ShaderResource, ImageFormat::RGBA})),
	_title(title + "##" + std::to_string(counter++))
{
}

ViewportPanel::ViewportPanel(const std::string& title) :
	_fallback(Image::Create({100, 100, ImageUsage_ShaderResource, ImageFormat::RGBA})),
	_title(title + "##" + std::to_string(counter++))
{
}

ViewportPanel::ViewportPanel() :
	_title(std::string("Viewport") + "##" + std::to_string(counter++))
{
}

void ViewportPanel::OnUi()
{
	auto toUse = _image.expired() ? _fallback : _image.lock();

	if (toUse == nullptr)
	{
		return;
	}

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

	ImGui::Image(&toUse->ShaderView(), {_viewportSize.x, _viewportSize.y});

	ImGui::End();
	ImGui::PopStyleVar();
}

void ViewportPanel::SetImage(const std::shared_ptr<Image>& image)
{
	_image = image;
}

auto ViewportPanel::MousePosition(bool normalized) const -> Vector2
{
	auto position = Mouse::CursorPosition(true);
	position.x -= _topLeft.x;
	position.y -= _topLeft.y;

	if (normalized)
	{
		const auto viewportWidth = _bottomRight.x - _topLeft.x;
		const auto viewportHeight = _bottomRight.y - _topLeft.y;
		return {position.x / viewportWidth * 2.0f - 1.0f, (position.y / viewportHeight * 2.0f - 1.0f) * -1.0f};
	}
	return position;
}
}
