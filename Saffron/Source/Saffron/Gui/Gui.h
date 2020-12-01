#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include "ImGuizmo.h"

#include "Saffron/Base.h"
#include "Saffron/Core/Math/SaffronMath.h"

namespace Se
{
using Font = ImFont;
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
	static void Shutdown();

	static void Begin();
	static void End();

	static void BeginPropertyGrid(float width = -1.0);
	static void EndPropertyGrid();

	static bool BeginTreeNode(const String &name, bool defaultOpen = true);
	static void EndTreeNode();

	static void Property(const String &name, const Function<void()> &onClick, bool secondColumn = false);
	static void Property(const String &name, const String &value);
	static bool Property(const String &name, String &value);
	static bool Property(const String &name, bool &value);
	static bool Property(const String &name, const String &text, const String &buttonName, const Function<void()> &onButtonPress);
	static bool Property(const String &name, int &value, int min = -1, int max = 1, float step = 1, PropertyFlag flags = PropertyFlag::None);
	static bool Property(const String &name, float &value, float min = -1.0f, float max = 1.0f, float step = 1.0f, PropertyFlag flags = PropertyFlag::None);
	static bool Property(const String &name, Vector2f &value, PropertyFlag flags);
	static bool Property(const String &name, Vector2f &value, float min = -1.0f, float max = 1.0f, float step = 1.0f, PropertyFlag flags = PropertyFlag::None);
	static bool Property(const String &name, Vector3f &value, PropertyFlag flags);
	static bool Property(const String &name, Vector3f &value, float min = -1.0f, float max = 1.0f, float step = 1.0f, PropertyFlag flags = PropertyFlag::None, Optional<Function<void()>> fn = {});
	static bool Property(const String &name, Vector4f &value, PropertyFlag flags);
	static bool Property(const String &name, Vector4f &value, float min = -1.0f, float max = 1.0f, float step = 1.0f, PropertyFlag flags = PropertyFlag::None);

	static void HelpMarker(const String &desc);
	static void InfoModal(const char *title, const char *text, bool &open);

	static int GetFontSize();
	static void SetStyle(Style style);
	static void SetFontSize(int size);

	static Font *AddFont(const Filepath &path, int size);

	static void ForceHideBarTab();

	static Vector4f GetSaffronOrange(float opacity = 1.0f);
	static Vector4f GetSaffronPurple(float opacity = 1.0f);

private:
	static void PushID();
	static void PopID();
	static Font *GetAppropriateFont(int size);

private:
	static Style m_CurrentStyle;
	static Map<int, ImFont *> m_Fonts;
	static Pair<int, ImFont *> m_CurrentFont;

};
}