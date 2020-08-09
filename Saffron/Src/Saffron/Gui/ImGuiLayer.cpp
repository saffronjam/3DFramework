#include "Saffron/SaffronPCH.h"

#include <GLFW/glfw3.h>

#include "Saffron/Core/Application.h"
#include "Saffron/Gui/ImGuiLayer.h"
#include "Saffron/Gui/ImGuiImpl.h"

namespace Se
{

ImGuiLayer::ImGuiLayer()
	: Layer("ImGui")
{
}

void ImGuiLayer::Begin()
{
	SE_PROFILE_FUNCTION();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
}

void ImGuiLayer::End()
{
	SE_PROFILE_FUNCTION();

	auto &io = ImGui::GetIO();
	auto &app = Application::Get();
	io.DisplaySize = ImVec2(static_cast<float>(app.GetWindow()->GetWidth()), static_cast<float>(app.GetWindow()->GetHeight()));

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
	{
		auto *backupContext = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backupContext);
	}
}

void ImGuiLayer::OnAttach()
{
	SE_PROFILE_FUNCTION();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	auto *context = ImGui::CreateContext();
	if ( !context )
	{
		SE_CORE_ASSERT(context, "Failed to create ImGui context");
	}

	auto &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	auto *pFont = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Roboto.ttf", 18.0f);
	io.FontDefault = io.Fonts->Fonts.back();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	auto &style = ImGui::GetStyle();
	if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, style.Colors[ImGuiCol_WindowBg].w);

	auto &app = Application::Get();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow *>(app.GetWindow()->GetNativeWindow()), true);
	ImGui_ImplOpenGL3_Init("#version 410");

}

void ImGuiLayer::OnDetach()
{
	SE_PROFILE_FUNCTION();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

}
