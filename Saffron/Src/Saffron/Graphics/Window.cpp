#include "Saffron/SaffronPCH.h"

#include "Saffron/Graphics/Window.h"


#ifdef SE_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsWindow.h"
#endif

namespace Se
{
Window::Window()
	: m_Width(0), m_Height(0), m_Position(0.0f, 0.0f)
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

const glm::vec2 &Window::GetPosition() const
{
	return m_Position;
}

Ref<Window> Window::Create(const WindowProps &props)
{
#ifdef SE_PLATFORM_WINDOWS
	return CreateScope<WindowsWindow>(props);
#else
	SE_CORE_ASSERT(false, "Unknown platform!");
	return nullptr;
#endif
}

}
