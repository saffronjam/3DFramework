#include "SaffronPCH.h"

#include <bgfx/bgfx.h>

#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Rendering/RenderGraph.h"
#include "Saffron/Rendering/Mesh.h"
#include "Saffron/Rendering/Material.h"

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

void Renderer::OnEvent(const Event& event)
{
	const EventDispatcher dispatcher(event);
	dispatcher.Try<WindowResizeEvent>(SE_BIND_EVENT_FN(Renderer::OnWindowResize));
}

void Renderer::SetViewId(Uint16 id)
{
	_viewId = id;
}

void Renderer::SetViewSize(Uint16 width, Uint16 height)
{
	bgfx::setViewRect(_viewId, 0, 0, width, height);
}

void Renderer::SetViewProjection(const Matrix& view, const Matrix& projection)
{
	bgfx::setViewTransform(_viewId, view.m, projection.m);
}

void Renderer::Submit(const Shared<Mesh>& mesh, const Shared<Program>& program, const Shared<Material>& material)
{
	mesh->_vertexBuffer->Bind();
	mesh->_indexBuffer->Bind();
	bgfx::submit(0, program->GetNativeHandle());
}

void Renderer::Execute()
{
	bgfx::touch(0);
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

bool Renderer::OnRendertargetResize(Uint32 width, Uint32 height)
{
	return false;
}

bool Renderer::OnWindowResize(const WindowResizeEvent& event)
{
	bgfx::reset(event.GetWidth(), event.GetHeight());
	return false;
}
}
