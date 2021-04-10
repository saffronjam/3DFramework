#include "SaffronPCH.h"

#include <bgfx/bgfx.h>

#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/RenderGraph.h"

namespace Se
{
Renderer* Renderer::_instance = nullptr;

Renderer::Renderer(const Shared<Window>& window) :
	_window(window)
{
	SE_CORE_ASSERT(_instance == nullptr, "Renderer was already instansiated");
	_instance = this;

	bgfx::Init initData;
	initData.platformData.nwh = _window->GetNativeWindow();
	bgfx::init(initData);
	bgfx::setViewRect(0, 0, 0, window->GetWidth(), window->GetHeight());
}


Renderer::~Renderer()
{
	_instance = nullptr;
}

void Renderer::Submit()
{
}

void Renderer::Execute()
{
#ifdef SE_DEBUG
	bgfx::frame(true);
#else
	bgfx::frame(false);
#endif
}

Renderer& Renderer::Instance()
{
	SE_CORE_ASSERT(_instance != nullptr, "Renderer was not instansiated")
	return *_instance;
}
}
