#include "SaffronPCH.h"

#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include "Saffron/Common/App.h"
#include "Saffron/Ui/Ui.h"


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
			ImGuizmo::BeginFrame();
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
			Instance()._pendingShaders.clear();
		}
	);
}

void Ui::BeginGizmo(uint viewportWidth, uint viewportHeight)
{
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, viewportWidth, viewportHeight);
}

void Ui::Image(const Texture& texture, const Vector2& size)
{
	ImVec2 imSize{size.x, size.y};
	if (size.x == 0.0f || size.y == 0.0f)
	{
		imSize = {static_cast<float>(texture.Width()), static_cast<float>(texture.Height())};
	}

	ImGui::Image(&const_cast<ID3D11ShaderResourceView&>(texture.ShaderView()), imSize);
}

void Ui::Image(const Texture& texture, const Shader& shader, const Vector2& size)
{
	ImVec2 imSize{size.x, size.y};
	if (size.x == 0.0f || size.y == 0.0f)
	{
		imSize = {static_cast<float>(texture.Width()), static_cast<float>(texture.Height())};
	}

	ImGui::Image(&const_cast<ID3D11ShaderResourceView&>(texture.ShaderView()), imSize);
}

void Ui::Image(const TextureCube& texture, const Vector2& size)
{
	ImVec2 imSize{ size.x, size.y };
	if (size.x == 0.0f || size.y == 0.0f)
	{
		imSize = { static_cast<float>(texture.Width()), static_cast<float>(texture.Height()) };
	}

	ImGui::Image(&const_cast<ID3D11ShaderResourceView&>(texture.ShaderView()), imSize);
}

void Ui::Image(const TextureCube& texture, const Shader& shader, const Vector2& size)
{
	ImVec2 imSize{ size.x, size.y };
	if (size.x == 0.0f || size.y == 0.0f)
	{
		imSize = { static_cast<float>(texture.Width()), static_cast<float>(texture.Height()) };
	}

	ImGui::Image(&const_cast<ID3D11ShaderResourceView&>(texture.ShaderView()), imSize);
}

void Ui::Image(const Se::Image& image, const Vector2& size)
{
	ImVec2 imSize{size.x, size.y};
	if (size.x == 0.0f || size.y == 0.0f)
	{
		imSize = {static_cast<float>(image.Width()), static_cast<float>(image.Height())};
	}

	ImGui::Image(&const_cast<ID3D11ShaderResourceView&>(image.ShaderView()), imSize);
}

void Ui::Image(const Se::Image& image, const Shader& shader, const Vector2& size)
{
	ImVec2 imSize{size.x, size.y};
	if (size.x == 0.0f || size.y == 0.0f)
	{
		imSize = {static_cast<float>(image.Width()), static_cast<float>(image.Height())};
	}

	const auto& inst = Instance()._pendingShaders.emplace_back(shader.ShareThisAs<const Shader>());

	ImGui::GetWindowDrawList()->AddCallback(shaderCallback, const_cast<void*>(reinterpret_cast<const void*>(&inst)));
	ImGui::Image(&const_cast<ID3D11ShaderResourceView&>(image.ShaderView()), imSize);
	ImGui::GetWindowDrawList()->AddCallback(ImDrawCallback_ResetRenderState, nullptr);
}

void Ui::Gizmo(Matrix& result, const Matrix& view, const Matrix proj, GizmoControl control)
{
	auto* model = reinterpret_cast<float*>(&result);
	const auto* viewPtr = reinterpret_cast<const float*>(&view);
	const auto* projPtr = reinterpret_cast<const float*>(&proj);

	ImGuizmo::Manipulate(viewPtr, projPtr, ToImGuizmoOperation(control), ImGuizmo::LOCAL, model);
}

ImGuizmo::OPERATION Ui::ToImGuizmoOperation(GizmoControl gizmoControl)
{
	switch (gizmoControl)
	{
	case GizmoControl::Translate: return ImGuizmo::TRANSLATE;
	case GizmoControl::Rotate: return ImGuizmo::ROTATE;
	case GizmoControl::Scale: return ImGuizmo::SCALE;
	default: ;
	}
}
}
