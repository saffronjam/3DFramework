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

	enum class PropertyFlag
	{
		None = 0, Color = 1, Drag = 2, Slider = 4
	};

public:
	static void Init();

	static bool Property(const std::string &name, bool &value);
	static bool Property(const std::string &name, float &value, float min = -1.0f, float max = 1.0f, PropertyFlag flags = PropertyFlag::None);
	static bool Property(const std::string &name, glm::vec2 &value, PropertyFlag flags);
	static bool Property(const std::string &name, glm::vec2 &value, float min = -1.0f, float max = 1.0f, PropertyFlag flags = PropertyFlag::None);
	static bool Property(const std::string &name, glm::vec3 &value, PropertyFlag flags);
	static bool Property(const std::string &name, glm::vec3 &value, float min = -1.0f, float max = 1.0f, PropertyFlag flags = PropertyFlag::None);
	static bool Property(const std::string &name, glm::vec4 &value, PropertyFlag flags);
	static bool Property(const std::string &name, glm::vec4 &value, float min = -1.0f, float max = 1.0f, PropertyFlag flags = PropertyFlag::None);

	static void HelpMarker(const std::string &desc);

	static void SetStyle(Style style);

private:
	static Style m_CurrentStyle;

};
}