#pragma once

#include "Saffron/Config.h"
#include "Saffron/Graphics/Layer.h"

namespace Saffron
{

class Window;

class SAFFRON_API ImGuiLayer : public Layer
{
public:
	LAYER_TYPE(ImGui);

public:
	explicit ImGuiLayer(Window &window);

	void Begin();
	void End();

	void OnAttach() override;
	void OnDetach() override;

	void OnEvent(const Event::Ptr pEvent) override;

};
}
