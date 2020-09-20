#pragma once

#include "SaffronPCH.h"
#include "Saffron/Core/Layer.h"

namespace Se {

class GuiLayer : public Layer
{
public:
	GuiLayer();
	GuiLayer(const std::string &name);
	virtual ~GuiLayer();

	void Begin();
	void End();

	void OnAttach() override;
	void OnDetach() override;
	void OnImGuiRender() override;
private:
	float m_Time = 0.0f;
};

}
