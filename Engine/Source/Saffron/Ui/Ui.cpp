#include "SaffronPCH.h"

#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include "Saffron/Common/App.h"
#include "Saffron/Ui/Ui.h"


namespace Se
{
Ui::Ui() :
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
}

Ui::~Ui()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Ui::BeginFrame()
{
	Renderer::Submit(
		[this](const RendererPackage& package)
		{
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
		}
	);
}

void Ui::EndFrame()
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
		}
	);
}
}
