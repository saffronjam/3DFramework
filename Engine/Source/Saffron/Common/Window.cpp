#include "SaffronPCH.h"

#include "Saffron/Common/Window.h"
#include "Saffron/Input/Mouse.h"


#ifdef SE_PLATFORM_WINDOWS
#include "Saffron/Platform/Windows/WindowsWindow.h"
#endif

namespace Se
{
SignalAggregate<const Event&> Window::Signals::OnEvent;

Window::Window(const Properties& properties) :
	_title(properties.Title),
	_position(properties.Position),
	_width(properties.Width),
	_height(properties.Height),
	_antiAliasing(properties.AntiAliasing)
{
}

void Window::HandleBufferedEvents()
{
	for (const auto& event : _events)
	{
		GetSignals().Emit<const Event&>(Signals::OnEvent, *event);
	}
	_events.clear();
}

Uint32 Window::GetWidth() const
{
	return _width;
}

Uint32 Window::GetHeight() const
{
	return _height;
}

const Vector2& Window::GetPosition() const
{
	return _position;
}

void Window::SetTitle(String title)
{
	PushEvent<WindowNewTitleEvent>(Move(title));
}

const String& Window::GetTitle() const
{
	return _title;
}

void Window::SetWindowIcon(Filepath filepath)
{
	PushEvent<WindowNewIconEvent>(Move(filepath));
}

void Window::SetAntiAliasing(AntiAliasing antiAliasing)
{
	PushEvent<WindowNewAntiAliasingEvent>(antiAliasing);
}

AntiAliasing Window::GetAntiAliasing() const
{
	return _antiAliasing;
}

Shared<Window> Window::Create(const Properties& properties)
{
	// TODO: Use RendererAPI::Current()
#ifdef SE_PLATFORM_WINDOWS
	return CreateShared<WindowsWindow>(properties);
#else
	SE_CORE_ASSERT(false, "Unknown platform!");
	return nullptr;
#endif
}
}
