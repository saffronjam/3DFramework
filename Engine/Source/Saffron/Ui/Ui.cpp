#include "SaffronPCH.h"

#include "Saffron/Common/Utils.h"
#include "Saffron/Ui/Ui.h"
#include "Saffron/Ui/Colors.h"

namespace Se
{
using namespace Ui;
using namespace Ui::Utils;
using namespace ImGuiUtils;
using namespace Draw;

std::atomic<int> IdBinder::counter = 0;

ImU32 Ui::Utils::ColourWithValue(const ImColor& color, float value)
{
	const ImVec4& colRow = color.Value;
	float hue, sat, val;
	ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, hue, sat, val);
	return ImColor::HSV(hue, sat, std::min(value, 1.0f));
}

ImU32 Ui::Utils::ColourWithSaturation(const ImColor& color, float saturation)
{
	const ImVec4& colRow = color.Value;
	float hue, sat, val;
	ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, hue, sat, val);
	return ImColor::HSV(hue, std::min(saturation, 1.0f), val);
}

ImU32 Ui::Utils::ColourWithHue(const ImColor& color, float hue)
{
	const ImVec4& colRow = color.Value;
	float h, s, v;
	ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, h, s, v);
	return ImColor::HSV(std::min(hue, 1.0f), s, v);
}

ImU32 Ui::Utils::ColourWithMultipliedValue(const ImColor& color, float multiplier)
{
	const ImVec4& colRow = color.Value;
	float hue, sat, val;
	ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, hue, sat, val);
	return ImColor::HSV(hue, sat, std::min(val * multiplier, 1.0f));
}

ImU32 Ui::Utils::ColourWithMultipliedSaturation(const ImColor& color, float multiplier)
{
	const ImVec4& colRow = color.Value;
	float hue, sat, val;
	ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, hue, sat, val);
	return ImColor::HSV(hue, std::min(sat * multiplier, 1.0f), val);
}

ImU32 Ui::Utils::ColourWithMultipliedHue(const ImColor& color, float multiplier)
{
	const ImVec4& colRow = color.Value;
	float hue, sat, val;
	ImGui::ColorConvertRGBtoHSV(colRow.x, colRow.y, colRow.z, hue, sat, val);
	return ImColor::HSV(std::min(hue * multiplier, 1.0f), sat, val);
}

auto Ui::Utils::ToSaffronRect(const ImRect& rect) -> FloatRect
{
	return {rect.Min.x, rect.Min.y, rect.Max.x - rect.Min.x, rect.Max.y - rect.Min.y};
}

void Draw::Underline(bool fullWidth, float offsetX, float offsetY)
{
	if (fullWidth)
	{
		if (ImGui::GetCurrentWindow()->DC.CurrentColumns != nullptr) ImGui::PushColumnsBackground();
		else if (ImGui::GetCurrentTable() != nullptr) ImGui::TablePushBackgroundChannel();
	}

	const float width = fullWidth ? ImGui::GetWindowWidth() : ImGui::GetContentRegionAvail().x;
	const ImVec2 cursor = ImGui::GetCursorScreenPos();
	ImGui::GetWindowDrawList()->AddLine(
		ImVec2(cursor.x + offsetX, cursor.y + offsetY),
		ImVec2(cursor.x + width, cursor.y + offsetY),
		Theme::backgroundDark,
		1.0f
	);

	if (fullWidth)
	{
		if (ImGui::GetCurrentWindow()->DC.CurrentColumns != nullptr) ImGui::PopColumnsBackground();
		else if (ImGui::GetCurrentTable() != nullptr) ImGui::TablePopBackgroundChannel();
	}
}

bool Draw::Vec3Control(const std::string& label, Vector3& values, float resetValue, float columnWidth)
{
	bool modified = false;


	IdBinder idBinder;

	ImGui::TableSetColumnIndex(0);
	ShiftCursor(17.0f, 7.0f);

	ImGui::Text(label.c_str());
	Underline(false, 0.0f, 2.0f);

	ImGui::TableSetColumnIndex(1);
	ShiftCursor(7.0f, 0.0f);

	{
		constexpr float spacingX = 8.0f;
		StyleBinder itemSpacing(ImGuiStyleVar_ItemSpacing, ImVec2{spacingX, 0.0f});
		StyleBinder padding(ImGuiStyleVar_WindowPadding, ImVec2{0.0f, 2.0f});

		{
			// Begin XYZ area
			ColorBinder padding(ImGuiCol_Border, IM_COL32(0, 0, 0, 0));
			StyleBinder frame(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 0));

			ImGui::BeginChild(
				ImGui::GetID((label + "fr").c_str()),
				ImVec2(ImGui::GetContentRegionAvail().x - spacingX, ImGui::GetFrameHeightWithSpacing() + 8.0f),
				false,
				ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
			);
		}
		constexpr float framePadding = 2.0f;
		constexpr float outlineSpacing = 1.0f;
		const float lineHeight = GImGui->Font->FontSize + framePadding * 2.0f;
		const ImVec2 buttonSize = {lineHeight + 2.0f, lineHeight};
		const float inputItemWidth = (ImGui::GetContentRegionAvail().x - spacingX) / 3.0f - buttonSize.x;

		const ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		auto drawControl = [&](
			const std::string& label,
			float& value,
			const ImVec4& colourN,
			const ImVec4& colourH,
			const ImVec4& colourP
		)
		{
			{
				StyleBinder buttonFrame(ImGuiStyleVar_FramePadding, ImVec2(framePadding, 0.0f));
				StyleBinder buttonRounding(ImGuiStyleVar_FrameRounding, 1.0f);
				ColorStackBinder buttonColours(
					ImGuiCol_Button,
					colourN,
					ImGuiCol_ButtonHovered,
					colourH,
					ImGuiCol_ButtonActive,
					colourP
				);

				//Ui::ScopedFont buttonFont(boldFont);

				ShiftCursorY(2.0f);
				if (ImGui::Button(label.c_str(), buttonSize))
				{
					value = resetValue;
					modified = true;
				}
			}

			ImGui::SameLine(0.0f, outlineSpacing);
			ImGui::SetNextItemWidth(inputItemWidth);
			ShiftCursorY(-2.0f);
			modified |= ImGui::DragFloat(("##" + label).c_str(), &value, 0.1f, 0.0f, 0.0f, "%.2f");

			//if (!Ui::IsItemDisabled()) Ui::DrawItemActivityOutline(2.0f, true, Colours::Theme::accent);
		};


		drawControl(
			"X",
			values.x,
			ImVec4{0.8f, 0.1f, 0.15f, 1.0f},
			ImVec4{0.9f, 0.2f, 0.2f, 1.0f},
			ImVec4{0.8f, 0.1f, 0.15f, 1.0f}
		);

		ImGui::SameLine(0.0f, outlineSpacing);
		drawControl(
			"Y",
			values.y,
			ImVec4{0.2f, 0.7f, 0.2f, 1.0f},
			ImVec4{0.3f, 0.8f, 0.3f, 1.0f},
			ImVec4{0.2f, 0.7f, 0.2f, 1.0f}
		);

		ImGui::SameLine(0.0f, outlineSpacing);
		drawControl(
			"Z",
			values.z,
			ImVec4{0.1f, 0.25f, 0.8f, 1.0f},
			ImVec4{0.2f, 0.35f, 0.9f, 1.0f},
			ImVec4{0.1f, 0.25f, 0.8f, 1.0f}
		);

		ImGui::EndChild();
	}

	return modified;
}

auto Draw::PropertyGridHeader(const std::string& name, bool openByDefault) -> bool
{
	ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth |
		ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

	if (openByDefault) treeNodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;

	bool open = false;
	constexpr float framePaddingX = 6.0f;
	constexpr float framePaddingY = 6.0f; // affects height of the header

	StyleBinder headerRounding(ImGuiStyleVar_FrameRounding, 0.0f);
	StyleBinder headerPaddingAndHeight(ImGuiStyleVar_FramePadding, ImVec2{framePaddingX, framePaddingY});

	//Ui::PushID();
	ImGui::PushID(name.c_str());
	open = ImGui::TreeNodeEx("##dummy_id", treeNodeFlags, Se::Utils::ToUpper(name).c_str());
	//Ui::PopID();
	ImGui::PopID();

	return open;
}


void Ui::ShiftCursorX(float distance)
{
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + distance);
}

void Ui::ShiftCursorY(float distance)
{
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + distance);
}

void Ui::ShiftCursor(float x, float y)
{
	const ImVec2 cursor = ImGui::GetCursorPos();
	ImGui::SetCursorPos(ImVec2(cursor.x + x, cursor.y + y));
}

bool Ui::BeginPopup(const char* str_id, ImGuiWindowFlags flags)
{
	bool opened = false;
	if (ImGui::BeginPopup(str_id, flags))
	{
		opened = true;
		// Fill background wiht nice gradient
		const float padding = ImGui::GetStyle().WindowBorderSize;
		const auto windowRect = RectExpanded(ImGui::GetCurrentWindow()->Rect(), -padding, -padding);
		ImGui::PushClipRect(windowRect.Min, windowRect.Max, false);
		const ImColor col1 = ImGui::GetStyleColorVec4(ImGuiCol_PopupBg); // Colours::Theme::backgroundPopup;
		const ImColor col2 = ColourWithMultipliedValue(col1, 0.8f);
		ImGui::GetWindowDrawList()->AddRectFilledMultiColor(windowRect.Min, windowRect.Max, col1, col1, col2, col2);
		ImGui::GetWindowDrawList()->AddRect(windowRect.Min, windowRect.Max, ColourWithMultipliedValue(col1, 1.1f));
		ImGui::PopClipRect();

		// Popped in EndPopup()
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(0, 0, 0, 80));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1.0f, 1.0f));
	}

	return opened;
}

void Ui::EndPopup()
{
	ImGui::PopStyleVar(); // WindowPadding;
	ImGui::PopStyleColor(); // HeaderHovered;
	ImGui::EndPopup();
}

auto ImGuiUtils::GetItemRect() -> ImRect
{
	return ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
}

auto ImGuiUtils::RectExpanded(const ImRect& rect, float x, float y) -> ImRect
{
	ImRect result = rect;
	result.Min.x -= x;
	result.Min.y -= y;
	result.Max.x += x;
	result.Max.y += y;
	return result;
}

auto ImGuiUtils::RectOffset(const ImRect& rect, float x, float y) -> ImRect
{
	ImRect result = rect;
	result.Min.x += x;
	result.Min.y += y;
	result.Max.x += x;
	result.Max.y += y;
	return result;
}

auto ImGuiUtils::RectOffset(const ImRect& rect, ImVec2 xy) -> ImRect
{
	return RectOffset(rect, xy.x, xy.y);
}

void Ui::BeginGizmo(uint viewportWidth, uint viewportHeight)
{
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, viewportWidth, viewportHeight);
}

void Ui::Gizmo(Matrix& result, const Matrix& view, Matrix proj, GizmoControl control)
{
	auto* model = reinterpret_cast<float*>(&result);
	const auto* viewPtr = reinterpret_cast<const float*>(&view);
	const auto* projPtr = reinterpret_cast<const float*>(&proj);

	Manipulate(viewPtr, projPtr, ToImGuizmoOperation(control), ImGuizmo::LOCAL, model);
}

ImGuizmo::OPERATION Ui::Utils::ToImGuizmoOperation(GizmoControl gizmoControl)
{
	switch (gizmoControl)
	{
	case GizmoControl::Translate: return ImGuizmo::TRANSLATE;
	case GizmoControl::Rotate: return ImGuizmo::ROTATE;
	case GizmoControl::Scale: return ImGuizmo::SCALE;
	default:
	{
		throw SaffronException("Invalid Gizmo Control");
	}
	}
}
}
