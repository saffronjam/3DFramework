#include "SaffronPCH.h"

#include "StartupLayer.h"

namespace Se
{
StartupLayer::StartupLayer()
{
}

void StartupLayer::OnAttach(Shared <BatchLoader> &loader)
{
	m_TextureStore["Checkerboard"] = Factory::Create<Texture2D>("Editor/Checkerboard.tga");
	m_TextureStore["SelectorBG"] = Factory::Create<Texture2D>("Editor/SelectorBG.png");
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
	if ( ImGui::Begin("ProjectSelector##BackgroundPreview", nullptr, windowFlags) )
	{
		ImGui::PopStyleVar(3);

		const auto winSize = ImGui::GetWindowSize();
		const Vector2f textureSize = { static_cast<float>(m_TextureStore["SelectorBG"]->GetWidth()),
									  static_cast<float>(m_TextureStore["SelectorBG"]->GetHeight()) };
		const ImVec2 imgageSize = { winSize.y * (textureSize.x / textureSize.y), winSize.y };
		ImGui::SetCursorPos({ winSize.x - imgageSize.x, 0.0f });
		ImGui::Image(reinterpret_cast<ImTextureID>(m_TextureStore["SelectorBG"]->GetRendererID()), imgageSize,
					 { 0.0f, 0.0f }, { 1.0f, 1.0f }, { 0.6f, 0.6f, 0.6f, 1.0f });

		ImGui::SetCursorPos({ 0.0f, 0.0f });
		Gui::SetFontSize(36);
		ImGui::Text("");
		Gui::SetFontSize(72);
		ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.07f);
		ImGui::Text("Select Project");
		ImGui::Text("");
		Gui::SetFontSize(18);

		const ImVec2 buttonSize = { 180.0f, 50.0f };
		const float posY = ImGui::GetWindowPos().y + 140.0f;
		const float height = 4.0f * ImGui::GetWindowHeight() / 5.0f - buttonSize.y - 180.0f;
		if ( height > 0 )
		{
			ImGui::SetNextWindowSize({ ImGui::GetWindowWidth() / 2.0f, height });
			ImGui::SetNextWindowPos({ ImGui::GetWindowPos().x + ImGui::GetWindowWidth() * 0.07f, posY });

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 0));
			if ( ImGui::Begin("ProjectSelector##Selector", nullptr,
							  ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoNavFocus |
							  ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
							  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove) )
			{
				ImGui::PopStyleVar(3);
				ImGui::PopStyleColor(1);
				const auto &projectList = App::Instance().GetRecentProjectList();
				const DateTime currentDate;

				auto PutProjectItem = [this](const Shared <Project> &project) {
					OutputStringStream oss;

					ImGui::Text("");
					Gui::SetFontSize(28);
					const auto textHeight = ImGui::CalcTextSize("\n\n").y;

					oss << "\n\n##" << project->GetUUID();
					bool badProject = false;
					if ( ImGui::Selectable(oss.str().c_str(), project == m_SelectedProject) )
					{
						if ( project->IsValid() )
						{
							ProjectSelected.Invoke(project);
							m_SelectedProject = project;
						}
						else
						{
							badProject = true;
							Run::Later([project] {
								App::Instance().RemoveProject(project);
									   });
						}
					}
					Gui::InfoModal("Bad Project", "Project could not be loaded", badProject);

					oss.str("");
					oss.clear();

					const auto &date = project->LastOpened();
					ImGui::Text("");
					Gui::SetFontSize(24);
					ImGui::SameLine();
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() - textHeight);
					ImGui::Text("%s", project->GetName().c_str());
					oss << date.WeekdayString(true) << " " << date.ANSIDateString() << " " << date.TimeString();
					const float dateWidth = ImGui::CalcTextSize(oss.str().c_str()).x;
					ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvailWidth() - dateWidth);
					ImGui::TextColored({ 0.8f, 0.8f, 0.8f, 1.0f }, "%s", oss.str().c_str());

					ImGui::Text("\n");

					Gui::SetFontSize(18);
					ImGui::SameLine();
					ImGui::TextColored({ 0.5f, 0.5f, 0.5f, 1.0f }, "%s", project->GetProjectFilepath().string().c_str());

				};

				auto RecentOperator = [&currentDate](const Shared <Project> &project) {
					return project->LastOpened().Year() == currentDate.Year() &&
						project->LastOpened().Month() == currentDate.Month();
				};
				auto LastMonthOperator = [&currentDate](const Shared <Project> &project) {
					return
						project->LastOpened().Year() == currentDate.Year() &&
						project->LastOpened().Month() == currentDate.Month() - 1 ||
						// Take December-January into account
						project->LastOpened().Year() == currentDate.Year() - 1 &&
						project->LastOpened().Month() == 12 && currentDate.Month() == 1;
				};
				auto OlderOperator = [&currentDate, &RecentOperator, &LastMonthOperator](
					const Shared <Project> &project) {
						return !RecentOperator(project) && !LastMonthOperator(project) &&
							project->LastOpened() < currentDate;
				};

				const auto numRecent = static_cast<int>(std::count_if(projectList.begin(), projectList.end(),
																	  RecentOperator));
				const auto numLastMonth = static_cast<int>(std::count_if(projectList.begin(), projectList.end(),
																		 LastMonthOperator));
				const auto numOlder = static_cast<int>(std::count_if(projectList.begin(), projectList.end(),
																	 OlderOperator));

				const auto firstRecent = std::find_if(projectList.begin(), projectList.end(), RecentOperator);
				const auto firstLastMonth = std::find_if(projectList.begin(), projectList.end(), LastMonthOperator);
				const auto firstOlder = std::find_if(projectList.begin(), projectList.end(), OlderOperator);

				auto PutTreeNode = [&PutProjectItem, &projectList](const char *name, const auto &firstIter, int num,
																   bool open) {
																	   Gui::SetFontSize(24);
																	   const auto nodeFlags = open ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None;
																	   ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(0, 0, 0, 0));
																	   ImGui::PushStyleColor(ImGuiCol_HeaderActive, IM_COL32(0, 0, 0, 0));
																	   if ( ImGui::TreeNodeEx(name, nodeFlags) )
																	   {
																		   ImGui::PopStyleColor(2);
																		   Gui::SetFontSize(18);
																		   for ( auto iter = firstIter;
																				iter != projectList.end() && std::distance(firstIter, iter) < num; ++iter )
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

				ImGui::SetColumnWidth(-1, ImGui::GetWindowSize().x * 0.5f);

				PutTreeNode(" Recent", firstRecent, numRecent, true);
				PutTreeNode(" Last Month", firstLastMonth, numLastMonth, true);
				PutTreeNode(" Older", firstOlder, numOlder, false);

				ImGui::Columns(1);
			}
			else
			{
				ImGui::PopStyleVar(3);
				ImGui::PopStyleColor(1);
			}
			ImGui::End();
		}

		Gui::SetFontSize(22);
		ImGui::SetCursorPos({ ImGui::GetWindowWidth() * 0.09f, 4.0f * ImGui::GetWindowHeight() / 5.0f - buttonSize.y });
		if ( ImGui::Button("Create New", buttonSize) )
		{
			ImGui::OpenPopup("##NewProjectModal");
		}

		ImGui::SetCursorPos({ ImGui::GetWindowWidth() * 0.10f + buttonSize.x,
							 4.0f * ImGui::GetWindowHeight() / 5.0f - buttonSize.y });
		if ( ImGui::Button("Browse", buttonSize) )
		{
			auto filepath = FileIOManager::OpenFile({ "Saffron Project (*.spr)", {"*.spr"} });
			if ( filepath.has_filename() && filepath.extension() == ".spr" )
			{
				auto importedProject = CreateShared<Project>(filepath);
				if ( importedProject->IsValid() )
				{
					App::Instance().AddProject(importedProject);
					App::Instance().SetActiveProject(importedProject);
					ProjectSelected.Invoke(importedProject);
				}
			}
		}

		Gui::SetFontSize(28);
		if ( ImGui::BeginPopupModal("##NewProjectModal", nullptr,
									ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize) )
		{
			static String projectName;
			char buffer[256];
			strcpy(buffer, projectName.c_str());

			if ( ImGui::InputTextWithHint("##NewProjectInputName", "Project name", buffer, 256) )
			{
				projectName = buffer;
			}

			ImGui::Columns(3, "ProjectTemplateSelector", false);
			const Filepath templateScenesDirectory = "Templates/Scenes";
			const auto templateSceneFilepaths = FileIOManager::GetFiles(templateScenesDirectory, ".ssc");
			static Filepath templateSceneChoice;
			for ( int i = 0; i < 3 && i < templateSceneFilepaths.size(); i++ )
			{
				if ( ImGui::Button(templateSceneFilepaths[i].path().stem().string().c_str()) )
				{
					templateSceneChoice = templateSceneFilepaths[i].path();
				}
			}
			ImGui::Columns(1);

			Gui::SetFontSize(22);
			if ( ImGui::Button("Create") && !projectName.empty() )
			{
				const auto projectFolder = "Projects/" + projectName;
				const auto projectFilepath = projectFolder + "/" + projectName + ".spr";
				const auto projectScenesFolder = projectFolder + "/Scenes";
				const auto projectStartupSceneFilepath = projectFolder + "/Scenes/" + projectName + ".ssc";

				FileIOManager::CreateDirectories(projectScenesFolder);
				FileIOManager::Copy(templateSceneChoice, projectStartupSceneFilepath);

				m_NewProject = CreateShared<Project>(projectName, DateTime{}, projectFilepath,
													 ArrayList < Filepath > {projectStartupSceneFilepath});

				ProjectSerializer serializer(*m_NewProject.value());
				serializer.Serialize(projectFilepath);

				App::Instance().AddProject(m_NewProject.value());
				App::Instance().SetActiveProject(m_NewProject.value());

				ProjectSelected.Invoke(m_NewProject.value());

				ImGui::CloseCurrentPopup();
				projectName = "";
				templateSceneChoice = "";
			}
			ImGui::SameLine();
			if ( ImGui::Button("Cancel") )
			{
				ImGui::CloseCurrentPopup();
				projectName = "";
				templateSceneChoice = "";
			}

			ImGui::EndPopup();
		}

		Gui::SetFontSize(18);

	}
	else
	{
		ImGui::PopStyleVar(3);
	}
	ImGui::End();
}

void StartupLayer::OnEvent(const Event &event)
{
}

}
