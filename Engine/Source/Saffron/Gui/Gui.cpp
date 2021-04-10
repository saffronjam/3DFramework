#include "SaffronPCH.h"

#include "Saffron/Common/App.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Rendering/Renderer.h"

namespace Se
{
Gui* Gui::_instance = nullptr;

Gui::Gui()
{
	SE_CORE_ASSERT(_instance == nullptr && "Gui was already instansiated");
	_instance = this;

	return;
	
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

	//auto *newFont = AddFont("Resources/Assets/Fonts/segoeui.ttf", 18);
	//io.FontDefault = newFont;
	//m_CurrentFont = { 18, newFont };

	//AddFont("Resources/Assets/Fonts/segoeui.ttf", 8);
	//AddFont("Resources/Assets/Fonts/segoeui.ttf", 12);
	//AddFont("Resources/Assets/Fonts/segoeui.ttf", 14);
	//AddFont("Resources/Assets/Fonts/segoeui.ttf", 20);
	//AddFont("Resources/Assets/Fonts/segoeui.ttf", 22);
	//AddFont("Resources/Assets/Fonts/segoeui.ttf", 24);
	//AddFont("Resources/Assets/Fonts/segoeui.ttf", 28);
	//AddFont("Resources/Assets/Fonts/segoeui.ttf", 32);
	//AddFont("Resources/Assets/Fonts/segoeui.ttf", 56);
	//AddFont("Resources/Assets/Fonts/segoeui.ttf", 72);

	App& app = App::Get();
	const auto hwnd = static_cast<HWND>(app.GetWindow()->GetNativeWindow());


	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
	}

	// Main Blue
	//const Vector3 mainVibrant = { 0.26f, 0.59f, 0.98f };
	//const Vector3 mainVibrantDark = { 0.24f, 0.52f, 0.88f };
	//const Vector3 mainLessVibrant = { 0.06f, 0.53f, 0.98f 

	// Main Orange
	//const Vector3 mainVibrant = { 0.89f, 0.46f, 0.16f };
	//const Vector3 mainVibrantDark = { 0.79f, 0.38f, 0.14f };
	//const Vector3 mainLessVibrant = { 0.89f, 0.39f, 0.02f };

	// Main Purple
	const Vector3 mainVibrant = {0.29f, 0.13f, 0.42f};
	const Vector3 mainVibrantDark = {0.19f, 0.15f, 0.23f};
	const Vector3 mainLessVibrant = {0.33f, 0.18f, 0.48f};

	const Vector4 mainNoTint(mainVibrant, 1.00f); //3	Main no tint
	const Vector4 mainTint1(mainVibrant, 0.95f); //9	Main tinted1
	const Vector4 mainTint2(mainVibrant, 0.80f); //8	Main tinted2
	const Vector4 mainTint3(mainVibrant, 0.67f); //2	Main tinted3
	const Vector4 mainTint4(mainVibrant, 0.40f); //1	Main tinted4
	const Vector4 mainTint5(mainVibrant, 0.35f); //13	Main tinted5

	const Vector4 mainDark = {mainVibrantDark, 1.00f}; //4	Main dark1 no tint

	const Vector4 mainLessVibrantNoTint = {mainLessVibrant, 1.00f}; //6	Less blue no tint
	const Vector4 mainLessVibrantTint1 = {mainLessVibrant, 0.60f}; //14 Less blue tinted1

	const Vector4 coMain = {1.00f, 0.43f, 0.35f, 1.00f}; //10	2ndMain no tint
	const Vector4 coMainDark = {0.90f, 0.70f, 0.00f, 1.00f}; //11	3rdMain no tint
	const Vector4 coMainRed = {1.00f, 0.60f, 0.00f, 1.00f}; //12	Co3rdMain 

	const auto ToImVec4 = [](const Vector4& vector)
	{
		return ImVec4{vector.x, vector.y, vector.z, vector.w};
	};

	style.Alpha = 1.0f;
	style.FrameRounding = 3.0f;
	style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	style.Colors[ImGuiCol_FrameBgHovered] = ToImVec4(mainTint4); //1
	style.Colors[ImGuiCol_FrameBgActive] = ToImVec4(mainTint3); //2
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ToImVec4(mainNoTint); //3
	style.Colors[ImGuiCol_SliderGrab] = ToImVec4(mainDark); //4
	style.Colors[ImGuiCol_SliderGrabActive] = ToImVec4(mainDark); //4
	style.Colors[ImGuiCol_Button] = ToImVec4(mainTint2); //1
	style.Colors[ImGuiCol_ButtonHovered] = ToImVec4(mainNoTint); //3
	style.Colors[ImGuiCol_ButtonActive] = ToImVec4(mainLessVibrantNoTint); //6
	style.Colors[ImGuiCol_Header] = ToImVec4(mainTint4); //7
	style.Colors[ImGuiCol_HeaderHovered] = ToImVec4(mainTint2); //8
	style.Colors[ImGuiCol_HeaderActive] = ToImVec4(mainNoTint); //3
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ToImVec4(mainTint3); //2
	style.Colors[ImGuiCol_ResizeGripActive] = ToImVec4(mainTint1); //9
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ToImVec4(coMain); //10
	style.Colors[ImGuiCol_PlotHistogram] = ToImVec4(coMainDark); //11
	style.Colors[ImGuiCol_PlotHistogramHovered] = ToImVec4(coMainRed); //12
	style.Colors[ImGuiCol_TextSelectedBg] = ToImVec4(mainTint5); //13
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
	style.Colors[ImGuiCol_Tab] = ToImVec4(mainTint4); //1
	style.Colors[ImGuiCol_TabHovered] = ToImVec4(mainNoTint); //3
	style.Colors[ImGuiCol_TabActive] = ToImVec4(mainLessVibrantNoTint); //6
	style.Colors[ImGuiCol_TabUnfocused] = ToImVec4(mainTint4); //1
	style.Colors[ImGuiCol_TabUnfocusedActive] = ToImVec4(mainLessVibrantNoTint);
}

Gui::~Gui()
{
	_instance = nullptr;
	return;
	ImGui::DestroyContext();
}

void Gui::Begin()
{
	return;
	ImGui::NewFrame();
}

void Gui::End()
{
	return;
	auto& app = App::Get();
	auto& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(static_cast<float>(app.GetWindow()->GetWidth()),
	                        static_cast<float>(app.GetWindow()->GetHeight()));


	ImGui::Render();
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
}

void Gui::SetStyle(GuiStyle style)
{
	return;
	ImGuiStyle& imguiStyle = ImGui::GetStyle();

	if (Instance()._currentStyle != style)
	{
		for (int i = 0; i <= ImGuiCol_COUNT; i++)
		{
			ImVec4& col = imguiStyle.Colors[i];
			float H, S, V;
			ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, H, S, V);

			if (S < 0.1f)
			{
				V = 1.0f - V;
			}
			ImGui::ColorConvertHSVtoRGB(H, S, V, col.x, col.y, col.z);
		}
		Instance()._currentStyle = style;
	}
}

Gui& Gui::Instance()
{
	SE_CORE_ASSERT(_instance != nullptr && "Gui was not instansiated");
	return *_instance;
}
}
