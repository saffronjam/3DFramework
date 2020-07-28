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

void Window::AddEventHandler(EventHandler *handler)
{
	m_handlers.emplace(handler);
}

void Window::HandleBufferedEvents()
{
	for ( auto &event : m_events )
	{
		for ( const auto &handler : m_handlers )
		{
			handler->OnEvent(*event);
		}
	}
	m_events.clear();
}

uint32_t Window::GetWidth() const
{
	return m_Width;
}

uint32_t Window::GetHeight() const
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
