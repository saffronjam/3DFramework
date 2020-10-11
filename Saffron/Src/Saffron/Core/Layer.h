#pragma once

#include "Saffron/Core/Time.h"
#include "Saffron/Core/Event.h"

namespace Se {

class Layer
{
public:
	Layer(const std::string &name = "Layer");
	virtual ~Layer();

	virtual void OnAttach() {}
	virtual void OnDetach() {}
	virtual void OnUpdate() {}
	virtual void OnImGuiRender() {}
	virtual void OnEvent(const Event &event) {}

	const std::string &GetName() const { return m_DebugName; }
protected:
	std::string m_DebugName;
};

}