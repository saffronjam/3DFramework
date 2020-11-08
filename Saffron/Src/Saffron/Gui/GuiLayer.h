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

	void OnAttach(Shared<BatchLoader> &loader) override;
	void OnDetach() override;
	void OnGuiRender() override;
};

}
