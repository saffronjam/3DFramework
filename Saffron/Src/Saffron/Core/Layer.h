#pragma once

#include "Saffron/Core/Time.h"
#include "Saffron/Core/Event.h"

namespace Se {

class Layer
{
public:
	Layer(const String &name = "Layer");
	virtual ~Layer();

	virtual void OnAttach() {}
	virtual void OnDetach() {}
	virtual void OnUpdate() {}
	virtual void OnGuiRender() {}
	virtual void OnEvent(const Event &event) {}

	const String &GetName() const { return m_DebugName; }
protected:
	String m_DebugName;
};

}