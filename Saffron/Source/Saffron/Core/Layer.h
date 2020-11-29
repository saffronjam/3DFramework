#pragma once

#include "Saffron/Core/Event.h"
#include "Saffron/Core/BatchLoader.h"

namespace Se {

class Layer : public MemManaged<Layer>
{
public:
	Layer(const String &name = "Layer");
	virtual ~Layer() = default;

	virtual void OnAttach(std::shared_ptr<BatchLoader> &loader) {}
	virtual void OnDetach() {}
	virtual void OnUpdate() {}
	virtual void OnGuiRender() {}
	virtual void OnEvent(const Event &event) {}

	const String &GetName() const { return m_DebugName; }
protected:
	String m_DebugName;
};

}