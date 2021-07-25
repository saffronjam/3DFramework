#include "SaffronPCH.h"

#include "Saffron/Common/App.h"

namespace Se
{
App::App(const AppSpec& spec) :
	_window(Window::Create(spec.WindowSpec))
{
}

void App::Run()
{
	while(true)
	{
		_window->DispatchEvents();
	}
}
}
