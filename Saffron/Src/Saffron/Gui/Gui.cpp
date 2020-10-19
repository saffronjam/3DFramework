#include "SaffronPCH.h"

#include "Saffron/Core/Math/SaffronMath.h"
#include "Saffron/Gui/Gui.h"

namespace Se
{

Gui::Style Gui::m_CurrentStyle = Style::Light;

///////////////////////////////////////////////////////////////////////////
/// Statics
///////////////////////////////////////////////////////////////////////////

static int s_UIContextID = 0;
static Uint32 s_Counter = 0;
static char s_IDBuffer[16];

void Gui::Init()
{
	ImGuiStyle &style = ImGui::GetStyle();

	style.Alpha = 1.0f;
	style.FrameRounding = 3.0f;
	style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.06f, 0.53f, 0.98f, 0.60f);

	SetStyle(Style::Dark);
}

void Gui::BeginPropertyGrid(float width)
{
	PushID();
	ImGui::Columns(2);
	ImGui::AlignTextToFramePadding();
}

void Gui::EndPropertyGrid()
{
	ImGui::Columns(1);
	PopID();
}

void Gui::SetStyle(Style style)
{
	ImGuiStyle &imguiStyle = ImGui::GetStyle();

	if ( m_CurrentStyle != style )
	{
		for ( int i = 0; i <= ImGuiCol_COUNT; i++ )
		{
			ImVec4 &col = imguiStyle.Colors[i];
			float H, S, V;
			ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, H, S, V);

			if ( S < 0.1f )
			{
				V = 1.0f - V;
			}
			ImGui::ColorConvertHSVtoRGB(H, S, V, col.x, col.y, col.z);
		}
		m_CurrentStyle = style;
	}
}

void Gui::PushID()
{
	ImGui::PushID(s_UIContextID++);
	s_Counter = 0;
}

void Gui::PopID()
{
	ImGui::PopID();
	s_UIContextID--;
}

void Gui::Property(const char *label, const char *value)
{
	ImGui::Text(label);
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	s_IDBuffer[0] = '#';
	s_IDBuffer[1] = '#';
	memset(s_IDBuffer + 2, 0, 14);
	_itoa(s_Counter++, s_IDBuffer + 2, 16);
	ImGui::InputText(s_IDBuffer, const_cast<char *>(value), 256, ImGuiInputTextFlags_ReadOnly);

	ImGui::PopItemWidth();
	ImGui::NextColumn();
}

bool Gui::Property(const char *label, std::string &value, bool error)
{
	bool modified = false;

	ImGui::Text(label);
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	char buffer[256];
	strcpy(buffer, value.c_str());

	s_IDBuffer[0] = '#';
	s_IDBuffer[1] = '#';
	memset(s_IDBuffer + 2, 0, 14);
	_itoa(s_Counter++, s_IDBuffer + 2, 16);

	if ( error )
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
	if ( ImGui::InputText(s_IDBuffer, buffer, 256) )
	{
		value = buffer;
		modified = true;
	}
	if ( error )
		ImGui::PopStyleColor();
	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return modified;
}

bool Gui::Property(const std::string &name, bool &value)
{
	ImGui::Text(name.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	const std::string id = "##" + name;
	const bool result = ImGui::Checkbox(id.c_str(), &value);

	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return result;
}

bool Gui::Property(const std::string &name, int &value, int min, int max, PropertyFlag flags)
{
	ImGui::Text(name.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	const std::string id = "##" + name;
	bool changed;
	if ( flags == PropertyFlag::Slider )
		changed = ImGui::SliderInt(id.c_str(), &value, min, max);
	else
		changed = ImGui::DragInt(id.c_str(), &value, 1.0f, min, max);

	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return changed;
}
bool Gui::Property(const std::string &name, float &value, float min, float max, PropertyFlag flags)
{
	ImGui::Text(name.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	const std::string id = "##" + name;
	bool changed;
	if ( flags == PropertyFlag::Slider )
		changed = ImGui::SliderFloat(id.c_str(), &value, min, max);
	else
		changed = ImGui::DragFloat(id.c_str(), &value, 1.0f, min, max);

	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return changed;
}

bool Gui::Property(const std::string &name, glm::vec2 &value, Gui::PropertyFlag flags)
{
	return Property(name, value, -1.0f, 1.0f, flags);
}

bool Gui::Property(const std::string &name, glm::vec2 &value, float min, float max, PropertyFlag flags)
{
	ImGui::Text(name.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	const std::string id = "##" + name;
	bool changed;
	if ( flags == PropertyFlag::Slider )
		changed = ImGui::SliderFloat2(id.c_str(), glm::value_ptr(value), min, max);
	else
		changed = ImGui::DragFloat2(id.c_str(), glm::value_ptr(value), 1.0f, min, max);

	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return changed;
}

bool Gui::Property(const std::string &name, glm::vec3 &value, PropertyFlag flags)
{
	return Property(name, value, -1.0f, 1.0f, flags);
}

bool Gui::Property(const std::string &name, glm::vec3 &value, float min, float max, PropertyFlag flags)
{
	ImGui::Text(name.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	const std::string id = "##" + name;
	bool changed;
	if ( static_cast<int>(flags) & static_cast<int>(PropertyFlag::Color) )
		changed = ImGui::ColorEdit3(id.c_str(), glm::value_ptr(value), ImGuiColorEditFlags_NoInputs);
	else if ( flags == PropertyFlag::Slider )
		changed = ImGui::SliderFloat3(id.c_str(), glm::value_ptr(value), min, max);
	else
		changed = ImGui::DragFloat3(id.c_str(), glm::value_ptr(value), 1.0f, min, max);

	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return changed;
}

bool Gui::Property(const std::string &name, glm::vec4 &value, PropertyFlag flags)
{
	return Property(name, value, -1.0f, 1.0f, flags);
}

bool Gui::Property(const std::string &name, glm::vec4 &value, float min, float max, PropertyFlag flags)
{
	ImGui::Text(name.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	const std::string id = "##" + name;
	bool changed;
	if ( static_cast<int>(flags) & static_cast<int>(PropertyFlag::Color) )
		changed = ImGui::ColorEdit4(id.c_str(), glm::value_ptr(value), ImGuiColorEditFlags_NoInputs);
	else if ( flags == PropertyFlag::Slider )
		changed = ImGui::SliderFloat4(id.c_str(), glm::value_ptr(value), min, max);
	else
		changed = ImGui::DragFloat4(id.c_str(), glm::value_ptr(value), 1.0f, min, max);

	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return changed;
}

void Gui::HelpMarker(const std::string &desc)
{
	ImGui::TextDisabled("(?)");
	if ( ImGui::IsItemHovered() )
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc.c_str());
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void Gui::InfoModal(const char *title, const char *text, bool open)
{
	if ( open )
	{
		PushID();

		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		ImGui::OpenPopup(title);

		if ( ImGui::BeginPopupModal(title) )
		{
			ImGui::Text(text);
			if ( ImGui::Button("Dismiss") )
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		PopID();
	}
}
}

