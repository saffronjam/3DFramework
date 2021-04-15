#include "SaffronPCH.h"

#include "Saffron/Core/Window.h"
#include "Saffron/Input/Mouse.h"


#ifdef SE_PLATFORM_WINDOWS
#include "Saffron/Platform/Windows/WindowsWindow.h"
#endif

namespace Se
{
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
		switch (event->GetType())
		{
#define CASE(source, eventType) case Event::Type::source ## eventType: {\
		const auto& castedEvent = static_cast<const source ## eventType ## Event&>(*event); \
		eventType.Invoke(castedEvent); \
		break; \
		}
		CASE(Window, Resized);
		CASE(Window, Moved);
		CASE(Window, Closed);
		CASE(Window, GainedFocus);
		CASE(Window, LostFocus);
		CASE(Window, DroppedFiles);
		CASE(Window, NewTitle);
		CASE(Window, NewIcon);
		CASE(Window, Maximized);
		CASE(Window, Minimized);
#undef CASE

#define CASE(source, eventType) case Event::Type::source ## eventType: {\
		const auto& castedEvent = static_cast<const source ## eventType ## Event&>(*event); \
		source ## eventType.Invoke(castedEvent); \
		break; \
		}
		CASE(Key, Pressed);
		CASE(Key, Released);
		CASE(Key, Repeated);
		CASE(Text, Input);

		CASE(MouseButton, Pressed);
		CASE(MouseButton, Released);
		CASE(MouseWheel, Scrolled);
		CASE(Mouse, Moved);
		CASE(Mouse, MovedRaw);
		CASE(Cursor, Entered);
		CASE(Cursor, Left);
#undef CASE
		default: break;
		}
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

const Vector2f& Window::GetPosition() const
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
}

AntiAliasing Window::GetAntiAliasing() const
{
	return _antiAliasing;
}

Shared<Window> Window::Create(const Properties& properties)
{
	// TODO: Use RendererAPI::Current()
#ifdef SE_PLATFORM_WINDOWS
	return Shared<WindowsWindow>::Create(properties);
#else
	SE_CORE_FALSE_ASSERT("Unknown platform!");
	return nullptr;
#endif
}
}
