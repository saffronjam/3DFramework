#include "StartupLayer.h"

StartupLayer::StartupLayer()
{
}

void StartupLayer::OnAttach(Shared<BatchLoader> &loader)
{
	m_TextureStore["Checkerboard"] = Texture2D::Create("Assets/Editor/Checkerboard.tga");
}

void StartupLayer::OnDetach()
{
}

void StartupLayer::OnUpdate()
{
}

void StartupLayer::OnGuiRender()
{
	ImGui::ShowDemoWindow();

	ImGuiViewport *viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	const auto windowFlags =
		ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoNavFocus |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	if ( ImGui::Begin("ProjectSelector", nullptr, windowFlags) )
	{
		Gui::SetFontSize(36);
		ImGui::Text("");
		Gui::SetFontSize(72);
		ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.07f);
		ImGui::Text(" Saffron Engine");
		ImGui::Text("");
		Gui::SetFontSize(18);

		const auto &projectList = Application::Get().GetProjectList();
		const DateTime currentDate;

		auto PutProjectItem = [this](const Application::Project &project)
		{
			const auto &date = project.LastOpened;
			OutputStringStream oss;

#define SIZE2WITH0 std::setw(2) << std::setfill('0')

			oss << project.Name << "\t" << date.WeekdayString(true) << " " << date.Year() << "-" << SIZE2WITH0 << date.Month() << "-" << SIZE2WITH0 << date.Day() << " " << SIZE2WITH0 << date.Hour() << ":" << SIZE2WITH0 << date.Minutes() << "\n\n";

			ImGui::Text("");
			Gui::SetFontSize(24);
			ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.07f + 10.0f);
			if ( ImGui::Selectable(oss.str().c_str(), &project == m_ChosenProject) ) { m_ChosenProject = &project; }
			Gui::SetFontSize(18);

			oss.str("");
			oss.clear();

			oss << "\n" << project.SceneFilepath.string();
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(120, 120, 120, 255));
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.07f + 10.0f);
			ImGui::Text("%s", oss.str().c_str());
			ImGui::PopStyleColor();
		};

		auto RecentOperator = [&currentDate](const Application::Project &project)
		{
			return	project.LastOpened.Year() == currentDate.Year() && project.LastOpened.Month() == currentDate.Month();
		};
		auto LastMonthOperator = [&currentDate](const Application::Project &project)
		{
			return
				(project.LastOpened.Year() == currentDate.Year() && project.LastOpened.Month() == currentDate.Month() - 1) ||
				// Take December-January into account
				(project.LastOpened.Year() == currentDate.Year() - 1 && project.LastOpened.Month() == 12 && currentDate.Month() == 1);
		};
		auto OlderOperator = [&currentDate, &RecentOperator, &LastMonthOperator](const Application::Project &project)
		{
			return !RecentOperator(project) && !LastMonthOperator(project) && project.LastOpened < currentDate;
		};

		const auto numRecent = std::count_if(projectList.begin(), projectList.end(), RecentOperator);
		const auto numLastMonth = std::count_if(projectList.begin(), projectList.end(), LastMonthOperator);
		const auto numOlder = std::count_if(projectList.begin(), projectList.end(), OlderOperator);

		const auto firstRecent = std::find_if(projectList.begin(), projectList.end(), RecentOperator);
		const auto firstLastMonth = std::find_if(projectList.begin(), projectList.end(), LastMonthOperator);
		const auto firstOlder = std::find_if(projectList.begin(), projectList.end(), OlderOperator);

		auto PutTreeNode = [&PutProjectItem, &projectList](const char *name, const auto &firstIter, int num, bool open)
		{
			Gui::SetFontSize(24);
			const auto nodeFlags = open ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None;
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, IM_COL32(0, 0, 0, 0));
			ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.07f);
			if ( ImGui::TreeNodeEx(name, nodeFlags) )
			{
				ImGui::PopStyleColor(2);
				Gui::SetFontSize(18);
				for ( auto iter = firstIter; iter != projectList.end() && std::distance(firstIter, iter) < num; ++iter )
				{
					PutProjectItem(*iter);
				}
				ImGui::TreePop();
			}
			else
			{
				ImGui::PopStyleColor(2);
			}
			Gui::SetFontSize(18);
			ImGui::Text("");
		};

		ImGui::Columns(2, nullptr, false);

		ImGui::SetColumnWidth(-1, ImGui::GetWindowSize().x * 0.6f);

		PutTreeNode(" Recent", firstRecent, numRecent, true);
		PutTreeNode(" Last Month", firstLastMonth, numLastMonth, true);
		PutTreeNode(" Older", firstOlder, numOlder, false);

		ImGui::Columns(1);


		const auto size = std::min(ImGui::GetWindowSize().x / 5.0f, ImGui::GetWindowSize().y / 5.0f);
		const ImVec2 imagePos = { 3.0f * ImGui::GetWindowSize().x / 4.0f - size / 2.0f, ImGui::GetWindowSize().y / 2.0f - size / 2.0f };
		ImGui::SetCursorPos(imagePos);
		const auto &texture = m_ChosenProject ? m_ChosenProject->PreviewTexture : m_TextureStore["Checkerboard"];
		ImGui::Image(reinterpret_cast<ImTextureID>(texture->GetRendererID()), { size, size }, { 0.0f, 0.0f }, { 1.0f, 1.0f });

	}
	ImGui::End();
	ImGui::PopStyleVar(3);
}

void StartupLayer::OnEvent(const Event &event)
{
}

