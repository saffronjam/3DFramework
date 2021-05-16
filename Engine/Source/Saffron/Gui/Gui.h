#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include "ImGuizmo.h"

#include "Saffron/Base.h"
#include "Saffron/Math/SaffronMath.h"

namespace Se
{
using Font = ImFont;

class Gui : public SingleTon<Gui>
{
public:
	enum class Style : int
	{
		DarkColorful,
		Dark,
		Light
	};

	enum class PropertyFlag
	{
		None = 0,
		Color = 1,
		Drag = 2,
		Slider = 4
	};

public:
	Gui();
	~Gui();

	void Begin();
	void End();

	static void BeginPropertyGrid(float width = -1.0);
	static void EndPropertyGrid();

	static bool BeginTreeNode(const String& label, bool defaultOpen = true);
	static void EndTreeNode();

	static void Property(const String& label, const Function<void()>& onClick, bool secondColumn = false);
	static void Property(const String& label, const String& value);
	static bool Property(const String& label, String& value, bool error = false);
	static bool Property(const String& label, bool& value);
	static bool Property(const String& label, const String& text, const String& buttonName,
	                     const Function<void()>& onButtonPress);
	static bool Property(const String& label, int& value, float step = 1, int min = 0, int max = 0,
	                     PropertyFlag flags = PropertyFlag::None);
	static bool Property(const String& label, float& value, float step = 0.1f, float min = 0.0f, float max = 0.0f,
	                     PropertyFlag flags = PropertyFlag::None);
	static bool Property(const String& label, Vector2& value, PropertyFlag flags);
	static bool Property(const String& label, Vector2& value, float step = 0.1f, float min = 0.0f, float max = 0.0f,
	                     PropertyFlag flags = PropertyFlag::None);
	static bool Property(const String& label, Vector3& value, PropertyFlag flags);
	static bool Property(const String& label, Vector3& value, float step = 0.1f, float min = 0.0f, float max = 0.0f,
	                     PropertyFlag flags = PropertyFlag::None);
	static bool Property(const String& label, Vector4& value, PropertyFlag flags);
	static bool Property(const String& label, Vector4& value, float step = 0.1f, float min = 0.0f, float max = 0.0f,
	                     PropertyFlag flags = PropertyFlag::None);

	static void HelpMarker(const String& desc);
	static void InfoModal(const char* title, const char* text, bool& open);

	static int GetFontSize();
	static void SetStyle(Style style);
	static void SetFontSize(int size);

	static Font* AddFont(const Path& filepath, int size);

	static void ForceHideBarTab();

	static Vector4 GetSaffronOrange(float opacity = 1.0f);
	static Vector4 GetSaffronPurple(float opacity = 1.0f);

private:
	static void PushID();
	static void PopID();
	static Font* GetAppropriateFont(int size);

	static void ApplyStyle(Style style);

private:
	Style _currentStyle;
	TreeMap<int, ImFont*> _fonts;
	Pair<int, ImFont*> _currentFont;

	char s_IDBuffer[16];
	uint s_Counter = 0;

	static constexpr const char* FontsLocation = "Assets/Fonts/";
	static constexpr const char* FontsExtension = ".ttf";
};
}
