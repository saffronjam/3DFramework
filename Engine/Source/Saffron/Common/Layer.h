#pragma once

#include "Saffron/Common/Event.h"
#include "Saffron/Common/BatchLoader.h"

namespace Se
{
class Layer : public MemManaged<Layer>
{
public:
	Layer(const String& name = "Layer");
	virtual ~Layer() = default;

	virtual void OnAttach(Shared<BatchLoader>& loader)
	{
	}

	virtual void OnDetach()
	{
	}

	virtual void OnUpdate()
	{
	}

	virtual void OnGuiRender()
	{
	}

	virtual void OnEvent(const Event& event)
	{
	}

	virtual void OnPreFrame()
	{
	}

	virtual void OnPostFrame()
	{
	}

	const String& GetName() const { return m_DebugName; }

protected:
	String m_DebugName;
};
}
