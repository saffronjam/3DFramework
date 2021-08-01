#pragma once

#include <memory>

#include "Saffron/Base.h"
#include "Saffron/Common/LayerStack.h"
#include "Saffron/Common/Timer.h"
#include "Saffron/Common/Window.h"
#include "Saffron/Input/Keyboard.h"
#include "Saffron/Input/Mouse.h"
#include "Saffron/Rendering/Renderer.h"
#include "Saffron/Ui/Ui.h"

// TEMP
#include "Saffron/Rendering/Bindables.h"
#include "Saffron/Ui/DockSpacePanel.h"
#include "Saffron/Ui/ViewportPanel.h"

namespace Se
{
struct AppSpec
{
	WindowSpec WindowSpec;
};

class App : public SingleTon<App>
{
public:
	explicit App(const AppSpec& spec);

	void Run();

	virtual void OnCreate() = 0;
	virtual void OnExit() = 0;
	virtual void OnUi();
	
	void OnClosed();

	auto Window() -> class Window&;
	auto Window() const -> const class Window&;
	auto Timer() const -> const class Timer&;

	void AttachLayer(const std::shared_ptr<Layer>& layer);

private:
	bool _running = true;

	std::unique_ptr<class Window> _window;
	std::unique_ptr<Renderer> _renderer;
	std::unique_ptr<Ui> _ui;

	std::unique_ptr<Keyboard> _keyboard;
	std::unique_ptr<Mouse> _mouse;

	LayerStack _layerStack;
	class Timer _timer;
};

std::unique_ptr<App> CreateApp();
}
