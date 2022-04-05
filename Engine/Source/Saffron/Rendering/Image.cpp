#include "SaffronPCH.h"

#include "Saffron/Rendering/Image.h"

#include "Saffron/ErrorHandling/ExceptionHelpers.h"
#include "Saffron/ErrorHandling/SaffronException.h"
#include "Saffron/Rendering/Renderer.h"

namespace Se
{
Image::Image(const ImageSpec& spec, const uint* initialData) :
	_spec(spec)
{
}

auto Image::Width() const -> uint
{
	return _spec.Height;
}

auto Image::Height() const -> uint
{
	return _spec.Width;
}

auto Image::Format() const -> ImageFormat
{
	return _spec.Format;
}

auto Image::Usage() const -> ImageUsage
{
	return _spec.Usage;
}

auto Image::Create(const ImageSpec& spec) -> std::shared_ptr<Image>
{
	return std::make_shared<Image>(spec);
}

auto Image::CreateFromBackBuffer() -> std::shared_ptr<Image>
{
	auto result = std::unique_ptr<Image>(new Image);

	return result;
}
}
