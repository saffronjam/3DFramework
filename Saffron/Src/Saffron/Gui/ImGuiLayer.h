#pragma once

#include "Saffron/Config.h"
#include "Saffron/Graphics/Layer.h"
#include "Saffron/Gui/Gui.h"

namespace Se
{

class Window;

class ImGuiLayer : public Layer
{
public:
	explicit ImGuiLayer(const Ref<Window> &pWindow);

	void Begin();
	void End();

	void OnAttach() override;
	void OnDetach() override;

};
}
