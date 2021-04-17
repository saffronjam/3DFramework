#include "SaffronPCH.h"

#include <GLFW/glfw3.h>

#include "Saffron/Core/App.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Gui/ImGuiImpl.h"

namespace Se
{
static int s_UIContextID = 0;

Gui::Gui() :
	SingleTon(this)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

	auto* newFont = AddFont("segoeui", 18);
	io.FontDefault = newFont;
	m_CurrentFont = {18, newFont};

	AddFont("segoeui", 8);
	AddFont("segoeui", 12);
	AddFont("segoeui", 14);
	AddFont("segoeui", 20);
	AddFont("segoeui", 22);
	AddFont("segoeui", 24);
	AddFont("segoeui", 28);
	AddFont("segoeui", 32);
	AddFont("segoeui", 56);
	AddFont("segoeui", 72);

	App& app = App::Instance();
	auto* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 410");

	SetStyle(Style::Dark);
}

Gui::~Gui()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	_instance = nullptr;
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
	ImGuiIO& io = ImGui::GetIO();
	App& app = App::Instance();
	io.DisplaySize = ImVec2(static_cast<float>(app.GetWindow().GetWidth()),
	                        static_cast<float>(app.GetWindow().GetHeight()));

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
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

bool Gui::BeginTreeNode(const String& label, bool defaultOpen)
{
	ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth |
		ImGuiTreeNodeFlags_FramePadding;
	if (defaultOpen) treeNodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;

	return ImGui::TreeNodeEx(label.c_str(), treeNodeFlags);
}

void Gui::EndTreeNode()
{
	ImGui::TreePop();
}

void Gui::Property(const String& label, const Function<void()>& onClick, bool secondColumn)
{
	if (secondColumn)
	{
		ImGui::NextColumn();
	}

	const String id = label + "##" + label;
	if (ImGui::Button(id.c_str(), {ImGui::GetContentRegionAvailWidth(), 0}))
	{
		onClick();
	}

	if (!secondColumn)
	{
		ImGui::NextColumn();
	}
	ImGui::NextColumn();
}

void Gui::Property(const String& label, const String& value)
{
	auto& inst = Instance();

	ImGui::Text(label.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	inst.s_IDBuffer[0] = '#';
	inst.s_IDBuffer[1] = '#';
	memset(inst.s_IDBuffer + 2, 0, 14);
	_itoa(inst.s_Counter++, inst.s_IDBuffer + 2, 16);
	ImGui::InputText(inst.s_IDBuffer, const_cast<char*>(value.c_str()), 256, ImGuiInputTextFlags_ReadOnly);

	ImGui::PopItemWidth();
	ImGui::NextColumn();
}

bool Gui::Property(const String& label, String& value, bool error)
{
	auto& inst = Instance();

	bool modified = false;

	ImGui::Text(label.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	char buffer[256];
	strcpy(buffer, value.c_str());

	inst.s_IDBuffer[0] = '#';
	inst.s_IDBuffer[1] = '#';
	memset(inst.s_IDBuffer + 2, 0, 14);
	_itoa(inst.s_Counter++, inst.s_IDBuffer + 2, 16);

	if (error) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
	if (ImGui::InputText(inst.s_IDBuffer, buffer, 256))
	{
		value = String(buffer);
		modified = true;
	}
	if (error) ImGui::PopStyleColor();
	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return modified;
}

bool Gui::Property(const String& label, bool& value)
{
	auto& inst = Instance();

	bool modified = false;

	ImGui::Text(label.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	inst.s_IDBuffer[0] = '#';
	inst.s_IDBuffer[1] = '#';
	memset(inst.s_IDBuffer + 2, 0, 14);
	_itoa(inst.s_Counter++, inst.s_IDBuffer + 2, 16);
	if (ImGui::Checkbox(inst.s_IDBuffer, &value)) modified = true;

	SE_CORE_INFO("Button with id: {0}", inst.s_IDBuffer);

	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return modified;
}

bool Gui::Property(const String& label, const String& text, const String& buttonName,
                   const Function<void()>& onButtonPress)
{
	auto& inst = Instance();

	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	const auto minButtonWidth = ImGui::CalcTextSize(buttonName.c_str()).x + 8.0f;
	const auto textBoxWidth = ImGui::GetContentRegionAvailWidth() - minButtonWidth;

	if (textBoxWidth > 0.0f)
	{
		ImGui::PushItemWidth(textBoxWidth);
		char buffer[256];
		strcpy(buffer, text.c_str());
		inst.s_IDBuffer[0] = '#';
		inst.s_IDBuffer[1] = '#';
		memset(inst.s_IDBuffer + 2, 0, 14);
		ImGui::InputText(inst.s_IDBuffer, const_cast<char*>(text.c_str()), 256, ImGuiInputTextFlags_ReadOnly);
		ImGui::PopItemWidth();
		ImGui::SameLine();
	}

	bool changed = false;
	const auto contentRegionAvailable = ImGui::GetContentRegionAvailWidth();
	if (contentRegionAvailable > 0.0f)
	{
		if (ImGui::Button(buttonName.c_str(), {ImGui::GetContentRegionAvailWidth(), 0.0f}))
		{
			if (onButtonPress) onButtonPress();
			changed = true;
		}
	}

	ImGui::NextColumn();

	return changed;
}

bool Gui::Property(const String& label, int& value, float step, int min, int max, PropertyFlag flags)
{
	auto& inst = Instance();

	bool changed;

	ImGui::Text(label.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	inst.s_IDBuffer[0] = '#';
	inst.s_IDBuffer[1] = '#';
	memset(inst.s_IDBuffer + 2, 0, 14);
	_itoa(inst.s_Counter++, inst.s_IDBuffer + 2, 16);
	if (flags == PropertyFlag::Slider) changed = ImGui::SliderInt(inst.s_IDBuffer, &value, min, max);
	else changed = ImGui::DragInt(inst.s_IDBuffer, &value, step, min, max);

	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return changed;
}

bool Gui::Property(const String& label, float& value, float step, float min, float max, PropertyFlag flags)
{
	auto& inst = Instance();

	bool changed;

	ImGui::Text(label.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	inst.s_IDBuffer[0] = '#';
	inst.s_IDBuffer[1] = '#';
	memset(inst.s_IDBuffer + 2, 0, 14);
	_itoa(inst.s_Counter++, inst.s_IDBuffer + 2, 16);
	if (flags == PropertyFlag::Slider) changed = ImGui::SliderFloat(inst.s_IDBuffer, &value, min, max);
	else changed = ImGui::DragFloat(inst.s_IDBuffer, &value, step, min, max);

	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return changed;
}

bool Gui::Property(const String& label, Vector2f& value, PropertyFlag flags)
{
	return Property(label, value, 0.1f, 0.0f, 0.0f, flags);
}

bool Gui::Property(const String& label, Vector2f& value, float step, float min, float max, PropertyFlag flags)
{
	auto& inst = Instance();

	bool changed;

	ImGui::Text(label.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	inst.s_IDBuffer[0] = '#';
	inst.s_IDBuffer[1] = '#';
	memset(inst.s_IDBuffer + 2, 0, 14);
	_itoa(inst.s_Counter++, inst.s_IDBuffer + 2, 16);
	if (flags == PropertyFlag::Slider) changed = ImGui::SliderFloat2(inst.s_IDBuffer, value_ptr(value), min, max);
	else changed = ImGui::DragFloat2(inst.s_IDBuffer, value_ptr(value), step, min, max);

	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return changed;
}

bool Gui::Property(const String& label, Vector3f& value, PropertyFlag flags)
{
	return Property(label, value, 0.1f, 0.0f, 0.0f, flags);
}

bool Gui::Property(const String& label, Vector3f& value, float step, float min, float max, PropertyFlag flags)
{
	auto& inst = Instance();

	bool changed;

	ImGui::Text(label.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	inst.s_IDBuffer[0] = '#';
	inst.s_IDBuffer[1] = '#';
	memset(inst.s_IDBuffer + 2, 0, 14);
	_itoa(inst.s_Counter++, inst.s_IDBuffer + 2, 16);
	if (static_cast<int>(flags) & static_cast<int>(PropertyFlag::Color))
		changed = ImGui::ColorEdit3(inst.s_IDBuffer, value_ptr(value), ImGuiColorEditFlags_NoInputs);
	else if (flags == PropertyFlag::Slider) changed = ImGui::SliderFloat3(inst.s_IDBuffer, value_ptr(value), min, max);
	else changed = ImGui::DragFloat3(inst.s_IDBuffer, value_ptr(value), step, min, max);

	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return changed;
}

bool Gui::Property(const String& label, Vector4f& value, PropertyFlag flags)
{
	return Property(label, value, 0.1f, 0.0f, 0.0f, flags);
}

bool Gui::Property(const String& label, Vector4f& value, float step, float min, float max, PropertyFlag flags)
{
	auto& inst = Instance();

	bool changed;

	ImGui::Text(label.c_str());
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	inst.s_IDBuffer[0] = '#';
	inst.s_IDBuffer[1] = '#';
	memset(inst.s_IDBuffer + 2, 0, 14);
	_itoa(inst.s_Counter++, inst.s_IDBuffer + 2, 16);
	if (static_cast<int>(flags) & static_cast<int>(PropertyFlag::Color))
		changed = ImGui::ColorEdit3(inst.s_IDBuffer, value_ptr(value), ImGuiColorEditFlags_NoInputs);
	else if (flags == PropertyFlag::Slider) changed = ImGui::SliderFloat4(inst.s_IDBuffer, value_ptr(value), min, max);
	else changed = ImGui::DragFloat4(inst.s_IDBuffer, value_ptr(value), step, min, max);

	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return changed;
}

void Gui::HelpMarker(const String& desc)
{
	auto& inst = Instance();

	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc.c_str());
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void Gui::InfoModal(const char* title, const char* text, bool& open)
{
	auto& inst = Instance();

	const auto prevFontSize = GetFontSize();
	SetFontSize(20);
	if (open && !ImGui::IsPopupOpen(title))
	{
		ImGui::OpenPopup(title);
	}

	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal(title, nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text(text);
		if (ImGui::Button("Dismiss"))
		{
			ImGui::CloseCurrentPopup();
		}
		open = false;
		ImGui::EndPopup();
	}
	SetFontSize(prevFontSize);
}

int Gui::GetFontSize()
{
	return Instance().m_CurrentFont.first;
}

void Gui::SetStyle(Style style)
{
	if (Instance().m_CurrentStyle != style)
	{
		ApplyStyle(style);
		Instance().m_CurrentStyle = style;
	}
}

void Gui::SetFontSize(int size)
{
	ImFont* candidate = GetAppropriateFont(size);
	SE_CORE_ASSERT(candidate, "Failed to fetch appropriate font and could be caused by an empty font container");
	ImGui::SetCurrentFont(candidate);
}

Font* Gui::AddFont(const Filepath& filepath, int size)
{
	const String fullFilepath = FontsLocation + filepath.string() + FontsExtension;
	auto* newFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(fullFilepath.c_str(), static_cast<float>(size));
	Instance().m_Fonts.emplace(size, newFont);
	return newFont;
}

void Gui::ForceHideBarTab()
{
	if (ImGui::IsWindowDocked())
	{
		auto* node = ImGui::GetWindowDockNode();
		if (node)
		{
			if (node && !node->IsHiddenTabBar())
			{
				node->WantHiddenTabBarToggle = true;
			}
		}
	}
}

Vector4f Gui::GetSaffronOrange(float opacity)
{
	return {0.89f, 0.46f, 0.16f, opacity};
}

Vector4f Gui::GetSaffronPurple(float opacity)
{
	return {0.29f, 0.13f, 0.42f, opacity};
}

void Gui::PushID()
{
	ImGui::PushID(s_UIContextID++);
	Instance().s_Counter = 0;
}

void Gui::PopID()
{
	ImGui::PopID();
	s_UIContextID--;
}

Font* Gui::GetAppropriateFont(int size)
{
	ImFont* candidate = nullptr;
	int bestDiff = std::numeric_limits<int>::max();
	for (auto& [fontSize, font] : Instance().m_Fonts)
	{
		if (std::abs(fontSize - size) > bestDiff)
		{
			break;
		}
		bestDiff = std::abs(fontSize - size);
		candidate = font;
	}
	return candidate;
}

void Gui::ApplyStyle(Style style)
{
	auto& io = ImGui::GetIO();

	auto& imGuiStyle = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		imGuiStyle.WindowRounding = 0.0f;
	}

	switch (style)
	{
	case Style::DarkColorful:
	{
		ApplyStyle(Style::Light);
		for (int i = 0; i <= ImGuiCol_COUNT; i++)
		{
			ImVec4& col = imGuiStyle.Colors[i];
			float H, S, V;
			ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, H, S, V);

			if (S < 0.1f)
			{
				V = 1.0f - V;
			}
			ImGui::ColorConvertHSVtoRGB(H, S, V, col.x, col.y, col.z);
		}

		break;
	}
	case Style::Dark:
	{
		ImGui::StyleColorsDark();

		auto& colors = imGuiStyle.Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{0.1f, 0.105f, 0.11f, 1.0f};

		// Headers
		colors[ImGuiCol_Header] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
		colors[ImGuiCol_HeaderHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
		colors[ImGuiCol_HeaderActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

		// Buttons
		colors[ImGuiCol_Button] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
		colors[ImGuiCol_ButtonHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
		colors[ImGuiCol_ButtonActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
		colors[ImGuiCol_FrameBgHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
		colors[ImGuiCol_FrameBgActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
		colors[ImGuiCol_TabHovered] = ImVec4{0.38f, 0.3805f, 0.381f, 1.0f};
		colors[ImGuiCol_TabActive] = ImVec4{0.28f, 0.2805f, 0.281f, 1.0f};
		colors[ImGuiCol_TabUnfocused] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};

		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
		colors[ImGuiCol_TitleBgActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

		// Resize Grip
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.91f, 0.91f, 0.91f, 0.25f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.81f, 0.81f, 0.81f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.46f, 0.46f, 0.46f, 0.95f);

		// Scrollbar
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.0f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.0f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.0f);

		// Check Mark
		colors[ImGuiCol_CheckMark] = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);

		// Slider
		colors[ImGuiCol_SliderGrab] = ImVec4(0.51f, 0.51f, 0.51f, 0.7f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.66f, 0.66f, 0.66f, 1.0f);
		break;
	}
	case Style::Light:
	{
		// Main Blue
		//const Vector3f mainVibrant = { 0.26f, 0.59f, 0.98f };
		//const Vector3f mainVibrantDark = { 0.24f, 0.52f, 0.88f };
		//const Vector3f mainLessVibrant = { 0.06f, 0.53f, 0.98f 

		// Main Orange
		//const Vector3f mainVibrant = { 0.89f, 0.46f, 0.16f };
		//const Vector3f mainVibrantDark = { 0.79f, 0.38f, 0.14f };
		//const Vector3f mainLessVibrant = { 0.89f, 0.39f, 0.02f };

		// Main Purple
		const Vector3f mainVibrant = {0.29f, 0.13f, 0.42f};
		const Vector3f mainVibrantDark = {0.19f, 0.15f, 0.23f};
		const Vector3f mainLessVibrant = {0.33f, 0.18f, 0.48f};

		const Vector4f mainNoTint = {mainVibrant, 1.00f}; //3	Main no tint
		const Vector4f mainTint1 = {mainVibrant, 0.95f}; //9	Main tinted1
		const Vector4f mainTint2 = {mainVibrant, 0.80f}; //8	Main tinted2
		const Vector4f mainTint3 = {mainVibrant, 0.67f}; //2	Main tinted3
		const Vector4f mainTint4 = {mainVibrant, 0.40f}; //1	Main tinted4
		const Vector4f mainTint5 = {mainVibrant, 0.35f}; //13	Main tinted5

		const Vector4f mainDark = {mainVibrantDark, 1.00f}; //4	Main dark1 no tint

		const Vector4f mainLessVibrantNoTint = {mainLessVibrant, 1.00f}; //6	Less blue no tint
		const Vector4f mainLessVibrantTint1 = {mainLessVibrant, 0.60f}; //14 Less blue tinted1

		const Vector4f coMain = {1.00f, 0.43f, 0.35f, 1.00f}; //10	2ndMain no tint
		const Vector4f coMainDark = {0.90f, 0.70f, 0.00f, 1.00f}; //11	3rdMain no tint
		const Vector4f coMainRed = {1.00f, 0.60f, 0.00f, 1.00f}; //12	Co3rdMain 

		const auto ToImVec4 = [](const Vector4f& vector)
		{
			return ImVec4{vector.x, vector.y, vector.z, vector.w};
		};

		auto& colors = imGuiStyle.Colors;

		imGuiStyle.Alpha = 1.0f;
		imGuiStyle.FrameRounding = 3.0f;
		colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
		colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
		colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
		colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
		colors[ImGuiCol_FrameBgHovered] = ToImVec4(mainTint4); //1
		colors[ImGuiCol_FrameBgActive] = ToImVec4(mainTint3); //2
		colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
		colors[ImGuiCol_CheckMark] = ToImVec4(mainNoTint); //3
		colors[ImGuiCol_SliderGrab] = ToImVec4(mainDark); //4
		colors[ImGuiCol_SliderGrabActive] = ToImVec4(mainDark); //4
		colors[ImGuiCol_Button] = ToImVec4(mainTint2); //1
		colors[ImGuiCol_ButtonHovered] = ToImVec4(mainNoTint); //3
		colors[ImGuiCol_ButtonActive] = ToImVec4(mainLessVibrantNoTint); //6
		colors[ImGuiCol_Header] = ToImVec4(mainTint4); //7
		colors[ImGuiCol_HeaderHovered] = ToImVec4(mainTint2); //8
		colors[ImGuiCol_HeaderActive] = ToImVec4(mainNoTint); //3
		colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
		colors[ImGuiCol_ResizeGripHovered] = ToImVec4(mainTint3); //2
		colors[ImGuiCol_ResizeGripActive] = ToImVec4(mainTint1); //9
		colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ToImVec4(coMain); //10
		colors[ImGuiCol_PlotHistogram] = ToImVec4(coMainDark); //11
		colors[ImGuiCol_PlotHistogramHovered] = ToImVec4(coMainRed); //12
		colors[ImGuiCol_TextSelectedBg] = ToImVec4(mainTint5); //13
		colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
		colors[ImGuiCol_Tab] = ToImVec4(mainTint4); //1
		colors[ImGuiCol_TabHovered] = ToImVec4(mainNoTint); //3
		colors[ImGuiCol_TabActive] = ToImVec4(mainLessVibrantNoTint); //6
		colors[ImGuiCol_TabUnfocused] = ToImVec4(mainTint4); //1
		colors[ImGuiCol_TabUnfocusedActive] = ToImVec4(mainLessVibrantNoTint); //14
		break;
	}
	}
}
}
