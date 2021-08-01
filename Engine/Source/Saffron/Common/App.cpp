#include "SaffronPCH.h"

#include "Saffron/Common/App.h"
#include "Saffron/Rendering/RendererApi.h"

namespace Se
{
App::App(const AppSpec& spec) :
	SingleTon(this),
	_window(Window::Create(spec.WindowSpec)),
	_renderer(std::make_unique<Renderer>(*_window)),
	_ui(std::make_unique<Ui>()),
	_keyboard(std::make_unique<Keyboard>()),
	_mouse(std::make_unique<Mouse>())
{
	_window->Closed += [this]
	{
		OnClosed();
		return false;
	};
	_renderer->Execute();
}

void App::Run()
{
	OnCreate();
	while (_running)
	{
		_keyboard->OnUpdate();
		_mouse->OnUpdate();
		_window->DispatchEvents();

		_renderer->BeginFrame();
		_ui->BeginFrame();

		for (auto& layer : _layerStack)
		{
			layer->OnUpdate(_timer.FrameTime());
		}

		Renderer::Submit(
			[this](const RendererPackage& package)
			{
				OnUi();
				for (auto& layer : _layerStack)
				{
					layer->OnUi();
				}
			}
		);

		_timer.Restart();
		_window->OnUpdate();
		_ui->EndFrame();
		_renderer->EndFrame();
	}

	for (auto& layer : _layerStack)
	{
		layer->OnDetach();
	}
	_layerStack.Clear();

	OnExit();
}

void App::OnUi()
{
	ImGui::Begin("App");
	ImGui::Columns(2);
	ImGui::Text("Vendor");
	ImGui::NextColumn();
	ImGui::Text("DirectX 11");
	ImGui::NextColumn();

	ImGui::Separator();

	ImGui::Text("Frame time");
	ImGui::NextColumn();
	ImGui::Text("%.12f ms", _timer.FrameTime().Ms());
	ImGui::NextColumn();	
	ImGui::NextColumn();
	const auto frameTime = _timer.FrameTime().Sec();
	if (frameTime == 0.0f)
	{
		ImGui::Text("NaN");
	}
	else
	{
		ImGui::Text("%.0f fps", 1.0f / frameTime);
	}
	ImGui::NextColumn();

	ImGui::End();
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

auto App::Timer() const -> const Se::Timer&
{
	return _timer;
}

void App::AttachLayer(const std::shared_ptr<Layer>& layer)
{
	_layerStack.Attach(layer);
}
}
