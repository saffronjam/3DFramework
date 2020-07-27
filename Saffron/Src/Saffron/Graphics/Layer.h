#pragma once

#include "Saffron/Config.h"
#include "Saffron/Event/Event.h"
#include "Saffron/Event/EventHandler.h"
#include "Saffron/System/Time.h"

namespace Saffron {

class SAFFRON_API Layer : public EventHandler
{
public:
	using Ptr = std::shared_ptr<Layer>;

public:
	Layer(std::string name = "Default");
	virtual ~Layer() = default;

	template<typename LayerType, typename ... Params>
	static Ptr Create(Params &&...params);

	virtual void OnAttach() {}
	virtual void OnDetach() {}
	virtual void OnUpdate(Time ts) {}
	virtual void OnImGuiRender() {}

	void OnEvent(const Event::Ptr pEvent) override {}

	const std::string &GetName() const { return m_DebugName; }
protected:
	std::string m_DebugName;
};

template<typename LayerType, typename ...Params>
Layer::Ptr Layer::Create(Params && ...params)
{
	return std::make_shared<LayerType>(std::forward(params...));
}

}