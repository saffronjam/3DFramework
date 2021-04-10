#pragma once

#include <ImGui/imgui.h>

#include "Saffron/Base.h"

namespace Se
{
enum class GuiStyle
{
	Dark,
	Light
};

class Gui
{
public:
	explicit Gui();
	Gui(const Gui&) = delete;
	Gui& operator=(const Gui&) = delete;
	~Gui();

	void Begin();
	void End();

	static bool Initialized() { return _instance != nullptr; }

	static void SetStyle(GuiStyle style);

private:
	static Gui& Instance();

private:
	static Gui* _instance;
	GuiStyle _currentStyle;
};
}
