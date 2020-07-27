#include "Saffron/Graphics/Layer.h"
#include "Saffron/Graphics/Window.h"

namespace Saffron
{

Layer::Layer(Window &window, std::string name)
	:
	m_window(window),
	m_DebugName(std::move(name))
{
	m_window.AddEventHandler(this);
}

}
