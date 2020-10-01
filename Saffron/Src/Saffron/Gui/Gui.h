#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include "ImGuizmo.h"

namespace Se
{
class Gui
{
public:
	enum class Style : int
	{
		Dark = 0, Light = 1
	};

public:
	static void Init();

	static void SetStyle(Style style);

private:
	static Style m_CurrentStyle;

};
}