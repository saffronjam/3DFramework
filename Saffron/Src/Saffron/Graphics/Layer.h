#pragma once

#include "Saffron/Config.h"
#include "Saffron/Event/Event.h"
#include "Saffron/Event/EventHandler.h"
#include "Saffron/Graphics/Window.h"
#include "Saffron/System/Time.h"

namespace Se
{
class Layer : public EventHandler
{
public:
	Layer(const Ref<Window> &pWindow, std::string name = "Default");
	virtual ~Layer() = default;

	template<typename LayerType, typename ... Params>
	static auto Create(class Window &window, Params &&...params);

	virtual void OnAttach() {}
	virtual void OnDetach() {}
	virtual void OnUpdate(Time ts) {}
	virtual void OnImGuiRender() {}

	void OnEvent(const Event &event) override {}

	const std::string &GetName() const { return m_DebugName; }
protected:
	Ref<Window> m_pWindow;
	std::string m_DebugName;
};

template<typename LayerType, typename ...Params>
auto Layer::Create(class Window &window, Params && ...params)
{
	return std::make_shared<LayerType>(window, std::forward<Params>(params)...);
}

}
