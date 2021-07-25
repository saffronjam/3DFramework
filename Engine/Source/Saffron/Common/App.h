#pragma once

#include <memory>

#include "Saffron/Base.h"
#include "Saffron/Common/Window.h"

namespace Se
{
struct AppSpec
{
	WindowSpec WindowSpec;
};

class App
{
public:
	explicit App(const AppSpec& spec);

	void Run();

private:
	std::unique_ptr<Window> _window;

};

std::unique_ptr<App> CreateApp();
}
