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
	LAYER_TYPE(ImGui);

public:
	explicit ImGuiLayer(const Window::Ptr &pWindow);

	void Begin();
	void End();

	void OnAttach() override;
	void OnDetach() override;

};
}
