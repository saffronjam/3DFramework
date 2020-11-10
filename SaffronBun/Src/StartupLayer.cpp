#include "StartupLayer.h"

StartupLayer::StartupLayer()
{
}

void StartupLayer::OnAttach(Shared<BatchLoader> &loader)
{
	m_TextureStore["Checkerboard"] = Texture2D::Create("Assets/Editor/Checkerboard.tga");
	m_CurrentPreviewTexture = m_TextureStore["Checkerboard"];
}

void StartupLayer::OnDetach()
{
}

void StartupLayer::OnUpdate()
{
}

void StartupLayer::OnGuiRender()
{
	ImGuiViewport *viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	const auto flags =
		ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::Begin("DockSpace Demo", nullptr, flags);
	ImGui::PopStyleVar(3);

	if ( ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable )
	{
		const ImGuiID dockspace_id = ImGui::GetID("StartupLayerDockspace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
	}

	ImGui::ShowDemoWindow();

	if ( ImGui::Begin("ProjectSelector##Headline") )
	{
		ImGui::SetWindowFontScale(10.0f);
		ImGui::Text("Test");
	}
	ImGui::End();

	if ( ImGui::Begin("ProjectSelector##Window", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse) )
	{
		ImGui::Columns(1, "ProjectSelector##Collumns");
		const auto &projectList = Application::Get().GetProjectList();
		static const Application::Project *selected = nullptr;
		for ( const auto &project : projectList )
		{
			ImGui::Separator();
			OutputStringStream oss;
			oss << project.Name << "\n" << project.SceneFilepath.string();
			if ( ImGui::Selectable(oss.str().c_str(), &project == selected) )
			{
				selected = &project;
				m_CurrentPreviewTexture = project.PreviewTexture;
			}
			ImGui::NextColumn();
		}
		ImGui::Separator();
		ImGui::Columns(1);
		if ( ImGui::IsWindowDocked() )
		{
			auto *wnd = ImGui::FindWindowByName("ProjectSelector##Window");
			if ( wnd )
			{
				ImGuiDockNode *node = wnd->DockNode;
				if ( node && !node->IsHiddenTabBar() )
				{
					node->WantHiddenTabBarToggle = true;
				}
			}
		}
	}
	ImGui::End();

	if ( ImGui::Begin("ProjectSelector##Image", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse) )
	{
		if ( m_CurrentPreviewTexture )
		{
			const auto size = std::min(ImGui::GetContentRegionAvailWidth(), ImGui::GetContentRegionAvail().y);
			const ImVec2 topLeft = { ImGui::GetWindowSize().x / 2.0f - size / 2.0f ,ImGui::GetWindowSize().y / 2.0f - size / 2.0f };
			ImGui::SetCursorPos(topLeft);
			ImGui::Image(reinterpret_cast<ImTextureID>(m_CurrentPreviewTexture->GetRendererID()), { size, size }, { 0.0f, 0.0f }, { 1.0f, 1.0f });
		}
		if ( ImGui::IsWindowDocked() )
		{
			auto *wnd = ImGui::FindWindowByName("ProjectSelector##Image");
			if ( wnd )
			{
				ImGuiDockNode *node = wnd->DockNode;
				if ( node && !node->IsHiddenTabBar() )
				{
					node->WantHiddenTabBarToggle = true;
				}
			}
		}
	}
	ImGui::End();



	ImGui::End();
}

void StartupLayer::OnEvent(const Event &event)
{
}

