#pragma once

#include <d3d11_4.h>

#include "Saffron/Base.h"
#include "Saffron/Common/Window.h"
#include "Saffron/Rendering/Bindables/Framebuffer.h"

namespace Se
{
class BackBuffer : public Framebuffer
{
public:
	explicit BackBuffer(const Window& window);

	void Resize(uint width, uint height) override;

	static auto Create(const Window& window) -> std::shared_ptr<BackBuffer>;

private:
	void OnWindowResize(const SizeEvent& event);
};
}
