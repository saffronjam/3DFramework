#pragma once

#include "SaffronPCH.h"
#include "Saffron/Core/Layer.h"

namespace Se {

class GuiLayer : public Layer
{
public:
	GuiLayer() = default;
	explicit GuiLayer(const String &name);
	virtual ~GuiLayer() = default;

	void Begin();
	void End();

	void OnAttach() override;
	void OnDetach() override;
	void OnGuiRender() override;
};

}
