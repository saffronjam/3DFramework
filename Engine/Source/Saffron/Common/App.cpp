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
	_window->Closed.Subscribe([this] { OnClosed(); });
}

void App::Run()
{
	while (_running)
	{
		_window->DispatchEvents();


		Renderer::Submit(
			[this](const RendererPackage& package)
			{
				RendererApi::Clear();
				_ui->Begin();
			}
		);

		
		_renderer->DrawTestTriangle();


		Renderer::Submit(
			[this](const RendererPackage& package)
			{
				ImGui::ShowDemoWindow();
				_ui->End();
				RendererApi::Present();
			}
		);


		_renderer->Execute();

		_window->OnUpdate();
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
