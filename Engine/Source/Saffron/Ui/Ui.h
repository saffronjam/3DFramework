#pragma once

#include <imgui.h>

#include "ImGuizmo.h"
#include "imgui_internal.h"

#include "Saffron/Base.h"
#include "Saffron/Graphics/Shapes.h"
#include "Saffron/Rendering/Bindables/Texture.h"
#include "Saffron/Rendering/Bindables/Shader.h"

namespace Se
{
enum class GizmoControl
{
	Translate,
	Rotate,
	Scale
};

namespace Ui
{
namespace ImGuiUtils
{
auto GetItemRect() -> ImRect;
auto RectExpanded(const ImRect& rect, float x, float y) -> ImRect;
auto RectOffset(const ImRect& rect, float x, float y) -> ImRect;
auto RectOffset(const ImRect& rect, ImVec2 xy) -> ImRect;
}

namespace Utils
{
auto ToImGuizmoOperation(GizmoControl gizmoControl) -> ImGuizmo::OPERATION;

#pragma region Binders

class StyleBinder
{
public:
	template <typename FloatOrVec2>
	StyleBinder(ImGuiStyleVar var, FloatOrVec2 val)
	{
		ImGui::PushStyleVar(var, val);
	}

	~StyleBinder()
	{
		ImGui::PopStyleVar();
	}
};

class ColorBinder
{
public:
	template <typename U32OrFloat4>
	ColorBinder(ImGuiStyleVar var, U32OrFloat4 val)
	{
		ImGui::PushStyleColor(var, val);
	}

	~ColorBinder()
	{
		ImGui::PopStyleColor();
	}
};

class IdBinder
{
public:
	explicit IdBinder()
	{
		ImGui::PushID(counter);
		counter = counter + 1;
	}

	~IdBinder()
	{
		ImGui::PopID();
		counter = counter - 1;
	}

private:
	static std::atomic<int> counter;
};

class ColorStackBinder
{
public:
	ColorStackBinder(const ColorStackBinder&) = delete;
	ColorStackBinder operator=(const ColorStackBinder&) = delete;

	template <typename ColourType, typename... OtherColours>
	ColorStackBinder(ImGuiCol firstColourID, ColourType firstColour, OtherColours&& ... otherColourPairs) :
		m_Count((sizeof...(otherColourPairs) / 2) + 1)
	{
		static_assert ((sizeof...(otherColourPairs) & 1u) == 0,
			"ColorStackBinder constructor expects a list of pairs of colour IDs and colours as its arguments");

		PushColour(firstColourID, firstColour, std::forward<OtherColours>(otherColourPairs)...);
	}

	~ColorStackBinder() { ImGui::PopStyleColor(m_Count); }

private:
	int m_Count;

	template <typename ColourType, typename... OtherColours>
	void PushColour(ImGuiCol colourID, ColourType colour, OtherColours&& ... otherColourPairs)
	{
		if constexpr (sizeof...(otherColourPairs) == 0)
		{
			ImGui::PushStyleColor(colourID, colour);
		}
		else
		{
			ImGui::PushStyleColor(colourID, colour);
			PushColour(std::forward<OtherColours>(otherColourPairs)...);
		}
	}
};

class StyleStackBinder
{
public:
	StyleStackBinder(const StyleStackBinder&) = delete;
	StyleStackBinder operator=(const StyleStackBinder&) = delete;

	template <typename ValueType, typename... OtherStylePairs>
	StyleStackBinder(ImGuiStyleVar firstStyleVar, ValueType firstValue, OtherStylePairs&& ... otherStylePairs) :
		m_Count((sizeof...(otherStylePairs) / 2) + 1)
	{
		static_assert ((sizeof...(otherStylePairs) & 1u) == 0,
			"StyleStackBinder constructor expects a list of pairs of colour IDs and colours as its arguments");

		PushStyle(firstStyleVar, firstValue, std::forward<OtherStylePairs>(otherStylePairs)...);
	}

	~StyleStackBinder() { ImGui::PopStyleVar(m_Count); }

private:
	int m_Count;

	template <typename ValueType, typename... OtherStylePairs>
	void PushStyle(ImGuiStyleVar styleVar, ValueType value, OtherStylePairs&& ... otherStylePairs)
	{
		if constexpr (sizeof...(otherStylePairs) == 0)
		{
			ImGui::PushStyleVar(styleVar, value);
		}
		else
		{
			ImGui::PushStyleVar(styleVar, value);
			PushStyle(std::forward<OtherStylePairs>(otherStylePairs)...);
		}
	}
};

#pragma endregion

#pragma region ColorConv
auto ColourWithValue(const ImColor& color, float value) -> ImU32;
auto ColourWithSaturation(const ImColor& color, float saturation) -> ImU32;
auto ColourWithHue(const ImColor& color, float hue) -> ImU32;
auto ColourWithMultipliedValue(const ImColor& color, float multiplier) -> ImU32;
auto ColourWithMultipliedSaturation(const ImColor& color, float multiplier) -> ImU32;
auto ColourWithMultipliedHue(const ImColor& color, float multiplier) -> ImU32;
#pragma endregion

#pragma region Rectangles
auto ToSaffronRect(const ImRect& rect) -> FloatRect;
#pragma endregion
}


namespace Draw
{
void Underline(bool fullWidth = false, float offsetX = 0.0f, float offsetY = -1.0f);

auto Vec3Control(
	const std::string& label,
	Vector3& values,
	float resetValue = 0.0f,
	float columnWidth = 100.0f
) -> bool;

#pragma region ButtonImage
void ButtonImage(
	const Texture& imageNormal,
	const Texture& imageHovered,
	const Texture& imagePressed,
	uint tintNormal,
	uint tintHovered,
	uint tintPressed,
	const Vector2& rectMin,
	const Vector2& rectMax
);
void ButtonImage(
	const Texture& image,
	uint tintNormal,
	uint tintHovered,
	uint tintPressed,
	const Vector2& rectMin,
	const Vector2& rectMax
);
void ButtonImage(const Texture& image, uint tintNormal, uint tintHovered, uint tintPressed, FloatRect rect);
void ButtonImage(
	const Texture& imageNormal,
	const Texture& imageHovered,
	const Texture& imagePressed,
	uint tintNormal,
	uint tintHovered,
	uint tintPressed
);
void ButtonImage(const Texture& image, uint tintNormal, uint tintHovered, uint tintPressed);
#pragma endregion

auto PropertyGridHeader(const std::string& name, bool openByDefault = true) -> bool
;
}

void BeginGizmo(uint viewportWidth, uint viewportHeight);

void Image(const Texture& texture, const Vector2& size = {0.0f, 0.0f});
void Image(const Texture& texture, const Shader& shader, const Vector2& size = {0.0f, 0.0f});
void Image(const TextureCube& texture, const Vector2& size = {0.0f, 0.0f});
void Image(const TextureCube& texture, const Shader& shader, const Vector2& size = {0.0f, 0.0f});
void Image(const class Image& image, const Vector2& size = {0.0f, 0.0f});
void Image(const class Image& image, const std::shared_ptr<Shader>& shader, const Vector2& size = {0.0f, 0.0f});

void Gizmo(Matrix& result, const Matrix& view, Matrix proj, GizmoControl control);

void ShiftCursorX(float distance);
void ShiftCursorY(float distance);
void ShiftCursor(float x, float y);

bool BeginPopup(const char* str_id, ImGuiWindowFlags flags = 0);
void EndPopup();
}
}
