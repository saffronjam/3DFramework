#include "SaffronPCH.h"

#include <GLFW/glfw3.h>

#include "Saffron/Core/Application.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Gui/ImGuiImpl.h"

namespace Se
{

Gui::Style Gui::m_CurrentStyle = Style::Light;
Map<int, ImFont *> Gui::m_Fonts;

static int s_UIContextID = 0;

void Gui::Init()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

	auto *newFont = AddFont("Assets/Fonts/segoeui.ttf", 18);
	io.FontDefault = newFont;

	AddFont("Assets/Fonts/segoeui.ttf", 8);
	AddFont("Assets/Fonts/segoeui.ttf", 12);
	AddFont("Assets/Fonts/segoeui.ttf", 14);
	AddFont("Assets/Fonts/segoeui.ttf", 24);
	AddFont("Assets/Fonts/segoeui.ttf", 32);
	AddFont("Assets/Fonts/segoeui.ttf", 56);
	AddFont("Assets/Fonts/segoeui.ttf", 72);

	Application &app = Application::Get();
	auto *window = static_cast<GLFWwindow *>(app.GetWindow().GetNativeWindow());

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 410");

	ImGui::StyleColorsDark();
	ImGuiStyle &style = ImGui::GetStyle();
	if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
	{
		style.WindowRounding = 0.0f;
	}
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

void Gui::Shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Gui::Begin()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
}

void Gui::End()
{
	ImGuiIO &io = ImGui::GetIO();
	Application &app = Application::Get();
	io.DisplaySize = ImVec2(static_cast<float>(app.GetWindow().GetWidth()), static_cast<float>(app.GetWindow().GetHeight()));

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
	{
		GLFWwindow *backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
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

void Gui::Property(const String &name, const Function<void()> &onClick, bool secondColumn)
{
	if ( secondColumn )
	{
		ImGui::NextColumn();
	}

	const String id = name + "##" + name;
	if ( ImGui::Button(id.c_str(), { ImGui::GetContentRegionAvailWidth(), 0 }) )
	{
		onClick();
	}

	if ( !secondColumn )
	{
		ImGui::NextColumn();
	}
	ImGui::NextColumn();
}

void Gui::Property(const String &name, const String &value)
{
	ImGui::Text(name.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	const String id = "##" + name;
	ImGui::InputText(id.c_str(), const_cast<char *>(value.c_str()), 256, ImGuiInputTextFlags_ReadOnly);

	ImGui::PopItemWidth();
	ImGui::NextColumn();
}

bool Gui::Property(const String &name, String &value)
{
	ImGui::Text(name.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	char buffer[256];
	strcpy(buffer, value.c_str());

	const String id = "##" + name;
	bool changed = false;
	if ( ImGui::InputText(id.c_str(), buffer, 256) )
	{
		value = buffer;
		changed = true;
	}
	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return changed;
}

bool Gui::Property(const String &name, bool &value)
{
	ImGui::Text(name.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	const String id = "##" + name;
	const bool result = ImGui::Checkbox(id.c_str(), &value);

	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return result;
}

bool Gui::Property(const String &name, const String &text, const String &buttonName, const Function<void()> &onButtonPress)
{
	ImGui::Text(name.c_str());
	ImGui::NextColumn();

	const auto minButtonWidth = ImGui::CalcTextSize(buttonName.c_str()).x + 8.0f;
	const auto textBoxWidth = ImGui::GetContentRegionAvailWidth() - minButtonWidth;

	if ( textBoxWidth > 0.0f )
	{
		ImGui::PushItemWidth(textBoxWidth);
		char buffer[256];
		strcpy(buffer, text.c_str());
		const String id = "##" + name;
		ImGui::InputText(id.c_str(), const_cast<char *>(text.c_str()), 256, ImGuiInputTextFlags_ReadOnly);
		ImGui::PopItemWidth();
		ImGui::SameLine();
	}

	bool changed = false;
	const auto contentRegionAvailable = ImGui::GetContentRegionAvailWidth();
	if ( contentRegionAvailable > 0.0f )
	{
		if ( ImGui::Button(buttonName.c_str(), { ImGui::GetContentRegionAvailWidth(), 0.0f }) )
		{
			if ( onButtonPress )
				onButtonPress();
			changed = true;
		}
	}

	ImGui::NextColumn();

	return changed;
}

bool Gui::Property(const String &name, int &value, int min, int max, float step, PropertyFlag flags)
{
	ImGui::Text(name.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	const String id = "##" + name;
	bool changed;
	if ( flags == PropertyFlag::Slider )
		changed = ImGui::SliderInt(id.c_str(), &value, min, max);
	else
		changed = ImGui::DragInt(id.c_str(), &value, step, min, max);

	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return changed;
}
bool Gui::Property(const String &name, float &value, float min, float max, float step, PropertyFlag flags)
{
	ImGui::Text(name.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	const String id = "##" + name;
	bool changed;
	if ( flags == PropertyFlag::Slider )
		changed = ImGui::SliderFloat(id.c_str(), &value, min, max);
	else
		changed = ImGui::DragFloat(id.c_str(), &value, step, min, max);

	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return changed;
}

bool Gui::Property(const String &name, Vector2f &value, Gui::PropertyFlag flags)
{
	return Property(name, value, -1.0f, 1.0f, 1.0f, flags);
}

bool Gui::Property(const String &name, Vector2f &value, float min, float max, float step, PropertyFlag flags)
{
	ImGui::Text(name.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	const String id = "##" + name;
	bool changed;
	if ( flags == PropertyFlag::Slider )
		changed = ImGui::SliderFloat2(id.c_str(), glm::value_ptr(value), min, max);
	else
		changed = ImGui::DragFloat2(id.c_str(), glm::value_ptr(value), step, min, max);

	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return changed;
}

bool Gui::Property(const String &name, Vector3f &value, PropertyFlag flags)
{
	return Property(name, value, -1.0f, 1.0f, 1.0f, flags);
}

bool Gui::Property(const String &name, Vector3f &value, float min, float max, float step, PropertyFlag flags)
{
	ImGui::Text(name.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);
	const String id = "##" + name;
	bool changed;
	if ( static_cast<int>(flags) & static_cast<int>(PropertyFlag::Color) )
		changed = ImGui::ColorEdit3(id.c_str(), glm::value_ptr(value), ImGuiColorEditFlags_NoInputs);
	else if ( flags == PropertyFlag::Slider )
		changed = ImGui::SliderFloat3(id.c_str(), glm::value_ptr(value), min, max);
	else
		changed = ImGui::DragFloat3(id.c_str(), glm::value_ptr(value), step, min, max);

	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return changed;
}

bool Gui::Property(const String &name, Vector4f &value, PropertyFlag flags)
{
	return Property(name, value, -1.0f, 1.0f, 1.0f, flags);
}

bool Gui::Property(const String &name, Vector4f &value, float min, float max, float step, PropertyFlag flags)
{
	ImGui::Text(name.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	const String id = "##" + name;
	bool changed;
	if ( static_cast<int>(flags) & static_cast<int>(PropertyFlag::Color) )
		changed = ImGui::ColorEdit4(id.c_str(), glm::value_ptr(value), ImGuiColorEditFlags_NoInputs);
	else if ( flags == PropertyFlag::Slider )
		changed = ImGui::SliderFloat4(id.c_str(), glm::value_ptr(value), min, max);
	else
		changed = ImGui::DragFloat4(id.c_str(), glm::value_ptr(value), step, min, max);

	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return changed;
}

void Gui::HelpMarker(const String &desc)
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
	if ( open && !ImGui::IsPopupOpen(title) )
	{
		ImGui::OpenPopup("Bad Entity Name");
	}

	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if ( ImGui::BeginPopupModal("Bad Entity Name", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize) )
	{
		ImGui::Text("Bad Entity Name");
		if ( ImGui::Button("Dismiss") )
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
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

void Gui::SetFontSize(int size)
{
	ImFont *candidate = GetAppropriateFont(size);
	SE_CORE_ASSERT(candidate, "Failed to fetch appropriate font and could be caused by an empty font container");
	ImGui::SetCurrentFont(candidate);
}

Font *Gui::AddFont(const Filepath &path, int size)
{
	auto *newFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(path.string().c_str(), static_cast<float>(size));
	m_Fonts.emplace(size, newFont);
	return newFont;
}

void Gui::ForceHideBarTab()
{
	if ( ImGui::IsWindowDocked() )
	{
		auto *node = ImGui::GetWindowDockNode();
		if ( node )
		{
			if ( node && !node->IsHiddenTabBar() )
			{
				node->WantHiddenTabBarToggle = true;
			}
		}
	}
}

void Gui::PushID()
{
	ImGui::PushID(s_UIContextID++);
}

void Gui::PopID()
{
	ImGui::PopID();
	s_UIContextID--;
}

Font *Gui::GetAppropriateFont(int size)
{
	ImFont *candidate = nullptr;
	int bestDiff = std::numeric_limits<int>::max();
	for ( auto &[fontSize, font] : m_Fonts )
	{
		if ( std::abs(fontSize - size) > bestDiff )
		{
			break;
		}
		bestDiff = std::abs(fontSize - size);
		candidate = font;
	}
	return candidate;
}
}

