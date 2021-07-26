#pragma once

#include <memory>

#include "Saffron/Base.h"
#include "Saffron/Common/Window.h"
#include "Saffron/Rendering/Renderer.h"

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

	void OnClosed();

	auto Window() -> class Window&;
	auto Window() const -> const class Window&;

private:
	bool _running = true;


	std::unique_ptr<class Window> _window;
	std::unique_ptr<Renderer> _renderer;
};

std::unique_ptr<App> CreateApp();
}
