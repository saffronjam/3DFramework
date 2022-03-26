#include "SaffronPCH.h"

#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include "Saffron/Common/App.h"
#include "Saffron/Ui/UiManager.h"
#include "Saffron/Ui/Colors.h"

namespace Se
{
static auto shaderCallback = [](const ImDrawList* list, const ImDrawCmd* cmd)
{
	const auto& userShader = *static_cast<std::shared_ptr<const Shader>*>(cmd->UserCallbackData);

	auto& ctx = ImGui_ImplDX11_GetBackendData()->pd3dDeviceContext;

	auto* drawData = ImGui::GetMainViewport()->DrawData;
	ImGui_ImplDX11_SetupRenderState(
		nullptr,
		ctx,
		&const_cast<ID3D11VertexShader&>(userShader->NativeVsHandle()),
		&const_cast<ID3D11PixelShader&>(userShader->NativePsHandle())
	);
};

UiManager::UiManager() :
	Singleton(this)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows

	const auto& app = App::Instance();
	const auto hwnd = static_cast<HWND>(app.Window().NativeHandle());
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(&Renderer::Device(), &Renderer::Context());

	auto& style = ImGui::GetStyle();
	auto& colors = ImGui::GetStyle().Colors;

	io.Fonts->AddFontFromFileTTF(
		"Assets/Fonts/Roboto/Roboto-Bold.ttf",
		18.0f,
		nullptr,
		io.Fonts->GetGlyphRangesCyrillic()
	);
	io.Fonts->AddFontFromFileTTF(
		"Assets/Fonts/Roboto/Roboto-Regular.ttf",
		24.0f,
		nullptr,
		io.Fonts->GetGlyphRangesCyrillic()
	);
	io.FontDefault = io.Fonts->AddFontFromFileTTF(
		"Assets/Fonts/Roboto/Roboto-SemiMedium.ttf",
		15.0f,
		nullptr,
		io.Fonts->GetGlyphRangesCyrillic()
	);

	//========================================================
	/// Colours

	// Headers
	colors[ImGuiCol_Header] = ImGui::ColorConvertU32ToFloat4(Theme::groupHeader);
	colors[ImGuiCol_HeaderHovered] = ImGui::ColorConvertU32ToFloat4(Theme::groupHeader);
	colors[ImGuiCol_HeaderActive] = ImGui::ColorConvertU32ToFloat4(Theme::groupHeader);

	// Buttons
	colors[ImGuiCol_Button] = ImColor(56, 56, 56, 200);
	colors[ImGuiCol_ButtonHovered] = ImColor(70, 70, 70, 255);
	colors[ImGuiCol_ButtonActive] = ImColor(56, 56, 56, 150);

	// Frame BG
	colors[ImGuiCol_FrameBg] = ImGui::ColorConvertU32ToFloat4(Theme::propertyField);
	colors[ImGuiCol_FrameBgHovered] = ImGui::ColorConvertU32ToFloat4(Theme::propertyField);
	colors[ImGuiCol_FrameBgActive] = ImGui::ColorConvertU32ToFloat4(Theme::propertyField);

	// Tabs
	colors[ImGuiCol_Tab] = ImGui::ColorConvertU32ToFloat4(Theme::titlebar);
	colors[ImGuiCol_TabHovered] = ImColor(255, 225, 135, 30);
	colors[ImGuiCol_TabActive] = ImColor(255, 225, 135, 60);
	colors[ImGuiCol_TabUnfocused] = ImGui::ColorConvertU32ToFloat4(Theme::titlebar);
	colors[ImGuiCol_TabUnfocusedActive] = colors[ImGuiCol_TabHovered];

	// Title
	colors[ImGuiCol_TitleBg] = ImGui::ColorConvertU32ToFloat4(Theme::titlebar);
	colors[ImGuiCol_TitleBgActive] = ImGui::ColorConvertU32ToFloat4(Theme::titlebar);
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
	colors[ImGuiCol_CheckMark] = ImColor(200, 200, 200, 255);

	// Slider
	colors[ImGuiCol_SliderGrab] = ImVec4(0.51f, 0.51f, 0.51f, 0.7f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.66f, 0.66f, 0.66f, 1.0f);

	// Text
	colors[ImGuiCol_Text] = ImGui::ColorConvertU32ToFloat4(Theme::text);

	// Checkbox
	colors[ImGuiCol_CheckMark] = ImGui::ColorConvertU32ToFloat4(Theme::text);

	// Separator
	colors[ImGuiCol_Separator] = ImGui::ColorConvertU32ToFloat4(Theme::backgroundDark);
	colors[ImGuiCol_SeparatorActive] = ImGui::ColorConvertU32ToFloat4(Theme::highlight);
	colors[ImGuiCol_SeparatorHovered] = ImColor(39, 185, 242, 150);

	// Window Background
	colors[ImGuiCol_WindowBg] = ImGui::ColorConvertU32ToFloat4(Theme::titlebar);
	colors[ImGuiCol_ChildBg] = ImGui::ColorConvertU32ToFloat4(Theme::background);
	colors[ImGuiCol_PopupBg] = ImGui::ColorConvertU32ToFloat4(Theme::backgroundPopup);
	colors[ImGuiCol_Border] = ImGui::ColorConvertU32ToFloat4(Theme::backgroundDark);

	// Tables
	colors[ImGuiCol_TableHeaderBg] = ImGui::ColorConvertU32ToFloat4(Theme::groupHeader);
	colors[ImGuiCol_TableBorderLight] = ImGui::ColorConvertU32ToFloat4(Theme::backgroundDark);

	// Menubar
	colors[ImGuiCol_MenuBarBg] = ImVec4{0.0f, 0.0f, 0.0f, 0.0f};

	//========================================================
	/// Style
	style.FrameRounding = 2.5f;
	style.FrameBorderSize = 1.0f;
	style.IndentSpacing = 11.0f;
}

UiManager::~UiManager()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void UiManager::BeginFrame()
{
	Renderer::Submit(
		[this](const RendererPackage& package)
		{
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
			ImGuizmo::BeginFrame();
		}
	);
}

void UiManager::EndFrame()
{
	Renderer::BackBuffer().Bind();
	Renderer::Submit(
		[this](const RendererPackage& package)
		{
			auto& app = App::Instance();
			auto& io = ImGui::GetIO();
			io.DisplaySize = ImVec2(
				static_cast<float>(app.Window().Width()),
				static_cast<float>(app.Window().Height())
			);

			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			Renderer::Instance().CleanDebugInfo();
			Instance()._pendingShaders.clear();
		}
	);
}

void UiManager::SetImageShader(const std::shared_ptr<Shader>& shader)
{
	_pendingShaders.emplace_back(shader);
	ImGui::GetWindowDrawList()->AddCallback(shaderCallback, const_cast<void*>(reinterpret_cast<const void*>(this)));
}

void UiManager::ResetImageShader()
{
	ImGui::GetWindowDrawList()->AddCallback(ImDrawCallback_ResetRenderState, nullptr);
}
}
