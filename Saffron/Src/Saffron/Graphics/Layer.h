#pragma once

#include "Saffron/Config.h"
#include "Saffron/Event/Event.h"
#include "Saffron/Event/EventHandler.h"
#include "Saffron/System/Time.h"

namespace Saffron
{
class Window;

class SAFFRON_API Layer : public EventHandler
{
public:
	using Ptr = std::shared_ptr<Layer>;

public:
	Layer(Window &window, std::string name = "Default");
	virtual ~Layer() = default;

	template<typename LayerType, typename ... Params>
	static auto Create(Params &&...params);

	virtual void OnAttach() {}
	virtual void OnDetach() {}
	virtual void OnUpdate(Time ts) {}
	virtual void OnImGuiRender() {}

	void OnEvent(const Event::Ptr pEvent) override {}

	const std::string &GetName() const { return m_DebugName; }
protected:
	Window &m_window;
	std::string m_DebugName;
};

template<typename LayerType, typename ...Params>
auto Layer::Create(Params && ...params)
{
	return std::make_shared<LayerType>(std::forward(params...));
}


#define LAYER_TYPE(x) using Ptr = std::shared_ptr<x##Layer>;

}
