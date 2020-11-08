#include "SaffronPCH.h"

#include "Saffron/Core/Window.h"


#ifdef SE_PLATFORM_WINDOWS
#include "Saffron/Platform/Windows/WindowsWindow.h"
#endif

namespace Se
{
Window::Window(const Properties &properties)
	:
	m_Title(properties.Title),
	m_Position(properties.Position),
	m_Width(properties.Width),
	m_Height(properties.Height)
{
}

void Window::HandleBufferedEvents()
{
	if ( m_EventCallback.has_value() )
	{
		for ( const auto &event : m_Events )
		{
			(*m_EventCallback)(*event);
		}
	}
	m_Events.clear();
}

void Window::SetEventCallback(const EventCallback &callback)
{
	m_EventCallback = callback;
}

Uint32 Window::GetWidth() const
{
	return m_Width;
}

Uint32 Window::GetHeight() const
{
	return m_Height;
}

const Vector2f &Window::GetPosition() const
{
	return m_Position;
}

void Window::SetTitle(String title)
{
	PushEvent<WindowNewTitleEvent>(Move(title));
}

const String &Window::GetTitle() const
{
	return m_Title;
}

void Window::SetWindowIcon(Filepath filepath)
{
	PushEvent<WindowNewIconEvent>(Move(filepath));
}

void Window::SetAntiAliasing(AntiAliasing antiAliasing)
{
	PushEvent<WindowNewAntiAliasingEvent>(antiAliasing);
}

AntiAliasing Window::GetAntiAliasing()const
{
	return m_AntiAliasing;
}

Shared<Window> Window::Create(const Properties &properties)
{
	// TODO: Use RendererAPI::Current()
#ifdef SE_PLATFORM_WINDOWS
	return Shared<WindowsWindow>::Create(properties);
#else
	SE_CORE_ASSERT(false, "Unknown platform!");
	return nullptr;
#endif
}

}
