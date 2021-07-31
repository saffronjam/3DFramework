#include "SaffronPCH.h"

#include "Saffron/Common/App.h"
#include "Saffron/Rendering/RendererApi.h"

namespace Se
{
App::App(const AppSpec& spec) :
	SingleTon(this),
	_window(Window::Create(spec.WindowSpec)),
	_renderer(std::make_unique<Renderer>(*_window)),
	_ui(std::make_unique<Ui>())
{
	_window->Closed += [this]
	{
		OnClosed();
		return false;
	};
}

void App::Run()
{
	while (_running)
	{
		_window->DispatchEvents();

		_renderer->BeginFrame();
		_ui->BeginFrame();

		_renderer->DrawTestTriangle();

		Renderer::Submit(
			[this](const RendererPackage& package)
			{
				ImGui::ShowDemoWindow();
			}
		);
		_window->OnUpdate();

		_ui->EndFrame();
		_renderer->EndFrame();
	}
}

void App::OnClosed()
{
	_running = false;
}

auto App::Window() -> Se::Window&
{
	return *_window;
}

auto App::Window() const -> const Se::Window&
{
	return const_cast<App&>(*this).Window();
}
}
