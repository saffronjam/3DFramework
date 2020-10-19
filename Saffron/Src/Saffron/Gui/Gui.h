#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include "ImGuizmo.h"

#include "Saffron/Base.h"

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


	static void BeginPropertyGrid(float width = -1.0);
	static void EndPropertyGrid();

	static void Property(const char *label, const char *value);
	static bool Property(const char *label, std::string &value, bool error = false);
	static bool Property(const std::string &name, bool &value);
	static bool Property(const std::string &name, int &value, int min = -1, int max = 1, PropertyFlag flags = PropertyFlag::None);
	static bool Property(const std::string &name, float &value, float min = -1.0f, float max = 1.0f, PropertyFlag flags = PropertyFlag::None);
	static bool Property(const std::string &name, glm::vec2 &value, PropertyFlag flags);
	static bool Property(const std::string &name, glm::vec2 &value, float min = -1.0f, float max = 1.0f, PropertyFlag flags = PropertyFlag::None);
	static bool Property(const std::string &name, glm::vec3 &value, PropertyFlag flags);
	static bool Property(const std::string &name, glm::vec3 &value, float min = -1.0f, float max = 1.0f, PropertyFlag flags = PropertyFlag::None);
	static bool Property(const std::string &name, glm::vec4 &value, PropertyFlag flags);
	static bool Property(const std::string &name, glm::vec4 &value, float min = -1.0f, float max = 1.0f, PropertyFlag flags = PropertyFlag::None);

	static void HelpMarker(const std::string &desc);
	static void InfoModal(const char *title, const char *text, bool open);

	static void SetStyle(Style style);

private:
	static void PushID();
	static void PopID();

private:
	static Style m_CurrentStyle;

};
}