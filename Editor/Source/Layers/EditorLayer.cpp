#include "SaffronPCH.h"

#include "EditorLayer.h"

namespace Se
{
EditorLayer::EditorLayer(const Shared<Project>& project) :
	m_Style(static_cast<int>(Gui::Style::Dark)),
	m_Project(Move(project)),
	m_MainViewportPane(CreateShared<ViewportPane>("Main Viewport", SceneRenderer::GetMainTarget())),
	m_LastFocusedScene(m_EditorScene),
	m_CachedActiveScene(m_EditorScene),
	m_SceneState(SceneState::Edit)
{
	App::Instance().GetWindow().SetAntiAliasing(AntiAliasing::Sample16);
}

void EditorLayer::OnAttach(Shared<BatchLoader>& loader)
{
	loader->Submit([this]
	{
		Gui::SetStyle(static_cast<Gui::Style>(m_Style));
	}, "Initializing GUI Style");

	loader->Submit([this]
	{
		m_TexStore["Checkerboard"] = Factory::Create<Texture2D>("Editor/Checkerboard.tga");
		m_TexStore["Checkerboard2"] = Factory::Create<Texture2D>("Editor/Checkerboard.tga");
		m_TexStore["PlayButton"] = Factory::Create<Texture2D>("Editor/PlayButton.png");
		m_TexStore["PauseButton"] = Factory::Create<Texture2D>("Editor/PauseButton.png");
		m_TexStore["StopButton"] = Factory::Create<Texture2D>("Editor/StopButton.png");
		m_TexStore["TranslateButton"] = Factory::Create<Texture2D>("Editor/Translate_w.png");
		m_TexStore["RotateButton"] = Factory::Create<Texture2D>("Editor/Rotate_w.png");
		m_TexStore["ScaleButton"] = Factory::Create<Texture2D>("Editor/Scale_w.png");
		m_TexStore["ControllerGameButton"] = Factory::Create<Texture2D>("Editor/ControllerGame_w.png");
		m_TexStore["ControllerMayaButton"] = Factory::Create<Texture2D>("Editor/ControllerMaya_w.png");
	}, "Loading Editor Textures");


	loader->Submit([this]
	{
		if (m_Project->IsValid())
		{
			m_AssetPanel = CreateShared<AssetPanel>(m_Project->GetProjectFolderpath().string() + "/Assets/Meshes");
			m_ScriptPanel = CreateShared<ScriptPanel>(m_Project->GetProjectFolderpath().string() + "/Src");
		}
		else
		{
			m_AssetPanel = CreateShared<AssetPanel>();
			m_ScriptPanel = CreateShared<ScriptPanel>();
		}
		m_EntityPanel = CreateShared<EntityPanel>(m_EditorScene);
		m_ScenePanel = CreateShared<ScenePanel>(m_EditorScene);
	}, "Initializing Editor Panels");

	loader->Submit([this]
	{
		OnProjectChange(m_Project);
	}, "Loading Default Scene");

	loader->Submit([this]
	{
		m_AssetScriptRunHandle = Run::Periodically([this]
		{
			m_AssetPanel->SyncAssetPaths();
			m_ScriptPanel->SyncScriptPaths();
		}, Time(1.0f));
	}, "Setting Up Periodic Callbacks");

	loader->Submit([this]
	{
		const auto postRenderFn = [this]()
		{
			const auto& scene = m_EditorScene;
			const auto& viewportPane = m_MainViewportPane;
			Entity modelEntity = m_SelectedEntity;

			if (m_SceneState == SceneState::Edit && m_GizmoType != -1 && GetActiveScene() == scene && modelEntity)
			{
				const auto viewportSize = viewportPane->GetViewportSize();
				const auto& editorCamera = scene->GetEntity().GetComponent<EditorCameraComponent>().Camera;

				const bool snap = Keyboard::IsPressed(KeyCode::LControl);
				auto& entityTransform = modelEntity.Transform();
				const float snapValue = GetSnapValue();
				float snapValues[3] = {snapValue, snapValue, snapValue};

				ImGuizmo::SetDrawlist();
				ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, viewportSize.x, viewportSize.y);


				static float bounds[] = {-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f};
				float* boundsPtr;
				if (m_SelectedEntity.HasComponent<MeshComponent>())
				{
					auto boundingBoxes = m_SelectedEntity.GetComponent<MeshComponent>().Mesh->GetBoundingBoxes(
						m_SelectedEntity.GetComponent<TransformComponent>().Transform);
					auto& first = boundingBoxes.front();
					bounds[0] = first.Min.x;
					bounds[1] = first.Min.y;
					bounds[2] = first.Min.z;
					bounds[3] = first.Max.x;
					bounds[4] = first.Max.y;
					bounds[5] = first.Max.z;
					boundsPtr = bounds;
				}
				else
				{
					boundsPtr = nullptr;
				}

				if (m_SelectionMode == SelectionMode::Entity)
				{
					ImGuizmo::Manipulate(glm::value_ptr(editorCamera->GetViewMatrix()),
					                     glm::value_ptr(editorCamera->GetProjectionMatrix()),
					                     static_cast<ImGuizmo::OPERATION>(m_GizmoType), ImGuizmo::WORLD,
					                     glm::value_ptr(entityTransform), nullptr, snap ? snapValues : nullptr,
					                     boundsPtr);
				}
				else
				{
					glm::mat4 transformBase = entityTransform * modelEntity.GetComponent<TransformComponent>().
					                                                        Transform;
					ImGuizmo::Manipulate(glm::value_ptr(editorCamera->GetViewMatrix()),
					                     glm::value_ptr(editorCamera->GetProjectionMatrix()),
					                     static_cast<ImGuizmo::OPERATION>(m_GizmoType), ImGuizmo::WORLD,
					                     glm::value_ptr(transformBase), nullptr, snap ? snapValues : nullptr,
					                     boundsPtr);

					modelEntity.GetComponent<TransformComponent>().Transform = glm::inverse(entityTransform) *
						transformBase;
				}
			}
		};

		m_MainViewportPane->FinishedRender += [postRenderFn]
		{
			postRenderFn();
			return false;
		};

		m_ScenePanel->EntityDeleted += SE_BIND_EVENT_FN(OnEntityDeleted);
		m_ScenePanel->EntityCopied += SE_BIND_EVENT_FN(OnEntityCopied);
		m_ScenePanel->ViewInModelSpace += SE_BIND_EVENT_FN(EditorLayer::OnNewModelSpaceView);
		m_ScenePanel->NewSelection += [this](Entity entity)
		{
			OnEntityUnselected(m_SelectedEntity);
			OnEntitySelected(entity);
			return false;
		};

		Keyboard::Pressed += SE_BIND_EVENT_FN(EditorLayer::OnKeyboardPress);
		Mouse::ButtonPressed += SE_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed);
		App::Instance().GetWindow().DroppedFiles += SE_BIND_EVENT_FN(EditorLayer::OnWindowDropFiles);
		
	}, "Setting Up System Callbacks");
}

void EditorLayer::OnDetach()
{
	if (m_SceneState == SceneState::Play)
	{
		OnStop();
	}
	Run::Remove(m_AssetScriptRunHandle);
}

void EditorLayer::OnUpdate()
{
	const auto activeScene = GetActiveScene();
	if (activeScene != m_CachedActiveScene)
	{
		OnSceneChange(activeScene, {});
	}

	switch (m_SceneState)
	{
	case SceneState::Edit:
	{
		m_MainViewportPane->IsFocused() ? m_EditorScene->EnableCamera() : m_EditorScene->DisableCamera();

		m_EditorScene->OnUpdate();
		m_EditorScene->OnRender();

		for (auto& [scene, viewportPane] : m_ModelSpaceSceneViews)
		{
			viewportPane->IsFocused() ? scene->EnableCamera() : scene->DisableCamera();

			scene->OnUpdate();
			scene->OnRender();
		}
		break;
	}
	case SceneState::Play:
	{
		m_RuntimeScene->OnUpdate();
		m_RuntimeScene->OnRender();
		break;
	}
	case SceneState::Pause:
	{
		break;
	}
	}
}

void EditorLayer::OnGuiRender()
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	const auto flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::Begin("EditorLayerDockspace", nullptr, flags);
	ImGui::PopStyleVar(3);

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		const ImGuiID dockspace_id = ImGui::GetID("EditorLayerDockspace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
	}

	OnGuiRenderMenuBar();
	OnGuiRenderToolbar();

	Renderer::OnGuiRender();
	SceneRenderer::OnGuiRender();
	ScriptEngine::OnGuiRender();
	Shader::OnGuiRender();
	m_EditorTerminal.OnGuiRender();
	m_AssetPanel->OnGuiRender();
	m_EntityPanel->OnGuiRender();
	m_ScriptPanel->OnGuiRender();
	m_ScenePanel->OnGuiRender(m_ScriptPanel);
	GetActiveScene()->OnGuiRender();
	m_MainViewportPane->OnGuiRender();

	if (m_SceneState == SceneState::Edit)
	{
		for (auto& [scene, viewportPane] : m_ModelSpaceSceneViews)
		{
			OutputStringStream oss;
			oss << scene->GetName() << "##" << scene->GetUUID();
			bool wantOpen;
			viewportPane->OnGuiRender(&wantOpen);
			if (!wantOpen)
			{
				const UUID sceneUUID = scene->GetUUID();
				Run::Later([sceneUUID, this]
				{
					const auto deleteIterator = std::find_if(m_ModelSpaceSceneViews.begin(),
					                                         m_ModelSpaceSceneViews.end(), [sceneUUID, this](auto& pair)
					                                         {
						                                         return pair.first->GetUUID() == sceneUUID;
					                                         });
					if (deleteIterator == m_ModelSpaceSceneViews.begin())
					{
						m_LastFocusedScene = std::dynamic_pointer_cast<Scene>(m_EditorScene);
					}
					else
					{
						m_LastFocusedScene = std::dynamic_pointer_cast<Scene>((deleteIterator - 1)->first);
					}
					m_ModelSpaceSceneViews.erase(deleteIterator);
				});
			}
		}
	}

	ImGui::End();

	// Correct scene viewport size after rendering viewport panes
	{
		const auto viewportSize = m_MainViewportPane->GetViewportSize();
		if (m_SceneState == SceneState::Play)
		{
			m_RuntimeScene->SetViewportSize(viewportSize.x, viewportSize.y);
		}
		m_EditorScene->SetViewportSize(viewportSize.x, viewportSize.y);
	}

	for (auto& [scene, viewportPane] : m_ModelSpaceSceneViews)
	{
		const auto viewportSize = viewportPane->GetViewportSize();
		scene->SetViewportSize(viewportSize.x, viewportSize.y);
	}
}

bool EditorLayer::OnKeyboardPress(const KeyPressedEvent& event)
{
	if (m_SceneState == SceneState::Edit && m_MainViewportPane->IsFocused())
	{
		switch (event.GetKey())
		{
		case KeyCode::Delete: if (m_SelectedEntity)
			{
				OnEntityDeleted(m_SelectedEntity);
			}
			break;
		default: break;
		}

		if (Keyboard::IsPressed(KeyCode::LControl))
		{
			switch (event.GetKey())
			{
			case KeyCode::C: if (m_SelectedEntity)
				{
					OnEntityCopied(m_SelectedEntity);
				}
				break;
			case KeyCode::G:
				// Toggle grid
				SceneRenderer::GetOptions().ShowGrid = !SceneRenderer::GetOptions().ShowGrid;
				break;
			case KeyCode::O: PromptImportScene();
				break;
			case KeyCode::S: SaveActiveScene();
				break;
			default: break;
			}

			if (Keyboard::IsPressed(KeyCode::LShift))
			{
				switch (event.GetKey())
				{
				case KeyCode::S: SaveActiveProject();
					break;
				default: break;
				}
			}
		}
	}

	return false;
}

bool EditorLayer::OnMouseButtonPressed(const MouseButtonPressedEvent& event)
{
	const auto& activeViewport = GetActiveViewportPane();
	const auto& activeScene = GetActiveScene();

	if (activeScene && activeViewport->IsFocused() && event.GetButton() == MouseButtonCode::Left && !
		Keyboard::IsPressed(KeyCode::LAlt) && !ImGuizmo::IsOver() && m_SceneState != SceneState::Play)
	{
		const auto MousePosition = activeViewport->GetMousePosition();
		if (MousePosition.x > -1.0f && MousePosition.x < 1.0f && MousePosition.y > -1.0f && MousePosition.y < 1.0f)
		{
			auto rayResult = CastRay(MousePosition.x, MousePosition.y);

			if (m_SelectedEntity)
			{
				OnEntityUnselected(m_SelectedEntity);
			}

			struct Selection
			{
				Entity Entity;
				const Submesh* Submesh = nullptr;
				float Distance = 0.0f;
			};

			ArrayList<Selection> selections;
			auto raycast = [&selections, &rayResult](Entity entity, const Shared<Mesh>& mesh)
			{
				const auto& submeshes = mesh->GetSubmeshes();
				for (Uint32 i = 0; i < submeshes.size(); i++)
				{
					const auto& submesh = submeshes[i];
					Ray ray = {
						glm::inverse(entity.Transform() * mesh->GetLocalTransform() * submesh.Transform) * Vector4f(
							rayResult.first, 1.0f),
						glm::inverse(
							Matrix3f(entity.Transform()) * Matrix3f(submesh.Transform) * Matrix3f(
								mesh->GetLocalTransform())) * rayResult.second
					};

					float t;
					const bool intersects = ray.IntersectsAABB(submesh.BoundingBox, t);
					if (intersects)
					{
						const auto& triangleCache = mesh->GetTriangleCache(i);
						for (const auto& triangle : triangleCache)
						{
							if (ray.IntersectsTriangle(triangle.V0.Position, triangle.V1.Position, triangle.V2.Position,
							                           t))
							{
								selections.push_back({entity, &submesh, t});
								break;;
							}
						}
					}
				}
			};

			auto meshEntities = GetActiveScene()->GetEntityRegistry().view<MeshComponent>();
			for (auto entityHandle : meshEntities)
			{
				const Entity entity = {entityHandle, m_EditorScene.get()};
				auto mesh = entity.GetComponent<MeshComponent>().Mesh;
				if (mesh)
				{
					raycast(entity, mesh);
				}
			}
			auto camereaEntities = GetActiveScene()->GetEntityRegistry().view<CameraComponent>();
			for (auto entityHandle : camereaEntities)
			{
				const Entity entity = {entityHandle, m_EditorScene.get()};
				auto cameraComponent = entity.GetComponent<CameraComponent>();
				if (cameraComponent.CameraMesh && cameraComponent.DrawMesh)
				{
					raycast(entity, cameraComponent.CameraMesh);
				}
			}
			std::sort(selections.begin(), selections.end(), [](auto& a, auto& b) { return a.Distance < b.Distance; });
			if (!selections.empty())
			{
				OnEntitySelected(selections.front().Entity);
			}
		}
	}
	return false;
}


bool EditorLayer::OnWindowDropFiles(const WindowDroppedFilesEvent& event)
{
	if (m_SceneState == SceneState::Edit)
	{
		for (const auto& filepath : event.GetFilepaths())
		{
			if (Scene::IsValidFilepath(filepath))
			{
				Run::Later([filepath, this]
				{
					const auto& transformedFilepath = m_Project->AddScene(filepath);
					LoadProjectScene(transformedFilepath);
				});
			}
		}
	}
	return false;
}

void EditorLayer::PromptNewScene()
{
	const Filepath filepath = FileIOManager::SaveFile({"Saffron Scene (*.ssc)", {"*.ssc"}});

	if (!filepath.empty())
	{
		OnStop();
		String sceneName = filepath.stem().string();
		Shared<EditorScene> newScene = CreateShared<EditorScene>(sceneName);

		// Default construct environment and light
		// TODO: Prompt user with templates instead?
		const Scene::Light light = {{-0.5f, -0.5f, 1.0f}, {1.0f, 1.0f, 1.0f}, 1.0f};
		newScene->SetLight(light);

		m_EditorScene = newScene;
		m_LastFocusedScene = std::dynamic_pointer_cast<Scene>(m_EditorScene);

		SaveActiveScene();
		OnSceneChange(m_EditorScene, {});

		m_Project->AddScene(filepath);
		m_Project->AddCachedScene(newScene);
	}
}

void EditorLayer::PromptNewProject()
{
	const Filepath filepath = FileIOManager::SaveFile({"Saffron Scene (*.spr)", {"*.spr"}});
}

void EditorLayer::PromptImportScene()
{
	const Filepath filepath = FileIOManager::OpenFile({"Saffron Scene (*.ssc)", {"*.ssc"}});
	if (Scene::IsValidFilepath(filepath))
	{
		const auto& retFilepath = m_Project->AddScene(Move(filepath));
		LoadProjectScene(filepath);
	}
}

void EditorLayer::PromptOpenProject()
{
	const Filepath filepath = FileIOManager::OpenFile({"Saffron Scene (*.spr)", {"*.spr"}});
	if (Project::IsValidProjectFilepath(filepath))
	{
		auto newProject = CreateShared<Project>(filepath);
		if (newProject->IsValid())
		{
			OnProjectChange(newProject);
		}
		else
		{
			SE_CORE_WARN("Failed to deserialize project");
		}
	}
}

void EditorLayer::SaveActiveProject() const
{
	for (const auto& cachedScene : m_Project->GetSceneCache())
	{
		cachedScene->Save();
	}
}

void EditorLayer::SaveActiveScene() const
{
	m_EditorScene->Save();
}

void EditorLayer::LoadProjectScene(const Filepath& filepath)
{
	if (!m_Project->IsValidSceneFilepath(filepath))
	{
		return;
	}
	const auto fullSceneFilepath = m_Project->GetFullSceneFilepath(filepath);

	Shared<EditorScene> newScene(nullptr);

	for (const auto& cachedScene : m_Project->GetSceneCache())
	{
		if (cachedScene->GetFilepath() == filepath)
		{
			newScene = cachedScene;
		}
	}

	if (!newScene)
	{
		newScene = CreateShared<EditorScene>(fullSceneFilepath);
		m_Project->AddCachedScene(newScene);
	}

	m_EditorScene = newScene;
	m_LastFocusedScene = std::dynamic_pointer_cast<Scene>(m_EditorScene);

	OnEntityUnselected(m_SelectedEntity);
	OnSceneChange(m_EditorScene, {});
}

void EditorLayer::OnSceneChange(Shared<Scene> scene, Entity selection)
{
	ScriptEngine::OnSceneChange(scene);
	m_EntityPanel->SetContext(scene);
	m_EntityPanel->SetSelectedEntity(selection ? selection : scene->GetSelectedEntity());
	m_ScenePanel->SetContext(scene);
	m_ScenePanel->SetSelectedEntity(selection ? selection : scene->GetSelectedEntity());
	m_SelectedEntity = selection ? selection : scene->GetSelectedEntity();
	m_CachedActiveScene = scene;
	m_LastFocusedScene = scene;
}

void EditorLayer::OnProjectChange(const Shared<Project>& project)
{
	SE_CORE_ASSERT(project->IsValid());
	ScriptEngine::OnProjectChange(project);
	m_Project = project;
	m_AssetPanel->SetAssetFolderpath(m_Project->GetProjectFolderpath().string() + "Assets/Meshes");
	m_ScriptPanel->SetAssetFolderpath(m_Project->GetProjectFolderpath().string() + "Source");

	LoadProjectScene(m_Project->GetSceneFilepaths().front());
}

void EditorLayer::OnWantProjectSelector()
{
	WantProjectSelector.Invoke();
}

void EditorLayer::OnPlay()
{
	m_SceneState = SceneState::Play;

	if (m_ReloadScriptOnPlay && m_Project->IsValid())
	{
		ScriptEngine::ReloadAssembly(m_Project->GetProjectFilepath().stem().string());
	}
	m_RuntimeScene = CreateShared<RuntimeScene>(m_EditorScene);
	m_RuntimeScene->OnStart();
	m_GizmoType = -1;

	// Swap selected entity to the copied entity in the runtime scene
	Entity transfer = {};
	if (m_SelectedEntity)
	{
		const auto& entityMap = m_RuntimeScene->GetEntityMap();
		const UUID selectedEntityUUID = m_SelectedEntity.GetUUID();
		if (entityMap.find(selectedEntityUUID) != entityMap.end()) transfer = entityMap.at(selectedEntityUUID);
	}
	OnSceneChange(m_RuntimeScene, transfer);
}

void EditorLayer::OnStop()
{
	m_SceneState = SceneState::Edit;
	m_RuntimeScene->OnStop();

	// Swap selected entity to the copied entity in the editor scene
	Entity transfer = {};
	if (m_SelectedEntity)
	{
		const auto& entityMap = m_EditorScene->GetEntityMap();
		const Se::UUID selectedEntityUUID = m_SelectedEntity.GetUUID();
		if (entityMap.find(selectedEntityUUID) != entityMap.end())
		{
			transfer = entityMap.at(selectedEntityUUID);
		}
	}
	OnSceneChange(m_EditorScene, transfer);

	// Unload runtime scene
	m_RuntimeScene = nullptr;
}

float EditorLayer::GetSnapValue() const
{
	switch (m_GizmoType)
	{
	case ImGuizmo::OPERATION::TRANSLATE: return 0.5f;
	case ImGuizmo::OPERATION::ROTATE: return 45.0f;
	case ImGuizmo::OPERATION::SCALE: return 0.5f;
	default: return 0.0f;
	}
}

Shared<ViewportPane> EditorLayer::GetActiveViewportPane() const
{
	if (m_SceneState == SceneState::Play)
	{
		return m_MainViewportPane;
	}

	const auto activeScene = GetActiveScene();
	if (activeScene == std::dynamic_pointer_cast<Scene>(m_EditorScene))
	{
		return m_MainViewportPane;
	}
	for (const auto& [scene, viewportPane] : m_ModelSpaceSceneViews)
	{
		if (activeScene == scene)
		{
			return viewportPane;
		}
	}
	SE_CORE_WARN("Detected a scene without a viewport pane. Scene name: {}", activeScene->GetName());
	return m_MainViewportPane;
}

Shared<Scene> EditorLayer::GetActiveScene() const
{
	if (m_SceneState == SceneState::Play)
	{
		m_LastFocusedScene = std::dynamic_pointer_cast<Scene>(m_RuntimeScene);
		return m_LastFocusedScene;
	}

	if (m_MainViewportPane->IsFocused())
	{
		m_LastFocusedScene = std::dynamic_pointer_cast<Scene>(m_EditorScene);
		return m_LastFocusedScene;
	}

	for (const auto& [scene, viewportPane] : m_ModelSpaceSceneViews)
	{
		if (viewportPane->IsFocused())
		{
			m_LastFocusedScene = std::dynamic_pointer_cast<Scene>(scene);
			return m_LastFocusedScene;
		}
	}

	return m_LastFocusedScene;
}


Pair<Vector3f, Vector3f> EditorLayer::CastRay(float mx, float my) const
{
	const Vector4f mouseClipPos = {mx, my, -1.0f, 1.0f};

	auto scene = GetActiveScene();
	if (scene->GetEntity().HasComponent<EditorCameraComponent>())
	{
		const auto& editorCamera = scene->GetEntity().GetComponent<EditorCameraComponent>().Camera;

		const auto inverseProj = glm::inverse(editorCamera->GetProjectionMatrix());
		const auto inverseView = glm::inverse(glm::mat3(editorCamera->GetViewMatrix()));

		const Vector4f ray = inverseProj * mouseClipPos;
		const Vector3f rayPos = editorCamera->GetPosition();
		const Vector3f rayDir = inverseView * Vector3f(ray);

		return {rayPos, rayDir};
	}
	return {};
}

void EditorLayer::OnGuiRenderMenuBar()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Scene", "Ctrl-N")) PromptNewScene();
			if (ImGui::MenuItem("New Project", "Ctrl-N")) PromptNewProject();

			ImGui::Separator();

			if (ImGui::MenuItem("Import Scene...", "Ctrl+I")) PromptImportScene();
			if (ImGui::MenuItem("Open Project...", "Ctrl+O")) PromptOpenProject();

			ImGui::Separator();

			if (ImGui::MenuItem("Save Scene", "Ctrl+S")) SaveActiveScene();
			if (ImGui::MenuItem("Save Project", "Ctrl+Shift+S")) SaveActiveProject();

			ImGui::Separator();

			if (ImGui::MenuItem("Open Project Selector")) OnWantProjectSelector();

			ImGui::Separator();
			if (ImGui::MenuItem("Exit"))
			{
				App::Instance().Exit();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Script"))
		{
			if (m_SceneState == SceneState::Play)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}
			if (ImGui::MenuItem("Reload C# Assembly") && m_Project->IsValid())
			{
				ScriptEngine::ReloadAssembly(m_Project->GetProjectFilepath().stem().string());
			}
			if (m_SceneState == SceneState::Play)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}

			ImGui::MenuItem("Reload assembly on play", nullptr, &m_ReloadScriptOnPlay);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Style"))
		{
			ImGui::RadioButton("Dark", &m_Style, 0);
			ImGui::RadioButton("Light", &m_Style, 1);
			ImGui::EndMenu();
		}
		Gui::SetStyle(static_cast<Gui::Style>(m_Style));

		ImGui::EndMenuBar();
	}
}

void EditorLayer::OnGuiRenderToolbar()
{
	// TODO: Move to header?
	const auto ImGuiBlue = ImVec4{0.137f, 0.263f, 0.424f, 1.0f};
	const auto ClearWhite = ImVec4{1.0f, 1.0f, 1.0f, 1.0f};
	const auto FadedGrey = ImVec4{0.3f, 0.3f, 0.3f, 0.5f};

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 4));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.8f, 0.8f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
	ImGui::Begin("Toolbar");


	Shared<Texture> ButtonTexture = m_SceneState == SceneState::Edit
		                                ? m_TexStore["PlayButton"]
		                                : m_TexStore["StopButton"];
	if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(ButtonTexture->GetRendererID()), ImVec2(25, 32), ImVec2(0, 0),
	                       ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ClearWhite))
	{
		m_SceneState == SceneState::Edit ? OnPlay() : OnStop();
	}

	ImGui::SameLine();
	ImGui::Separator();
	ImGui::SameLine();

	ImVec4 TranslateColorTint = m_GizmoType == ImGuizmo::OPERATION::TRANSLATE ? ImGuiBlue : ClearWhite;
	ImVec4 RotateColorTint = m_GizmoType == ImGuizmo::OPERATION::ROTATE ? ImGuiBlue : ClearWhite;
	ImVec4 ScaleColorTint = m_GizmoType == ImGuizmo::OPERATION::SCALE ? ImGuiBlue : ClearWhite;
	ImVec4 ControllerWASDTint;
	ImVec4 ControllerMayaTint;

	const auto& scene = GetActiveScene();
	if (scene && scene->GetEntity().HasComponent<EditorCameraComponent>())
	{
		const auto& editorCamera = scene->GetEntity().GetComponent<EditorCameraComponent>().Camera;
		ControllerWASDTint = editorCamera->GetControllerStyle() == EditorCamera::ControllerStyle::Game
			                     ? ImGuiBlue
			                     : ClearWhite;
		ControllerMayaTint = editorCamera->GetControllerStyle() == EditorCamera::ControllerStyle::Maya
			                     ? ImGuiBlue
			                     : ClearWhite;
	}
	else
	{
		TranslateColorTint = FadedGrey;
		RotateColorTint = FadedGrey;
		ScaleColorTint = FadedGrey;
		ControllerWASDTint = FadedGrey;
		ControllerMayaTint = FadedGrey;
	}


	if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(m_TexStore["TranslateButton"]->GetRendererID()),
	                       ImVec2(32, 32), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), TranslateColorTint))
	{
		if (m_SceneState == SceneState::Edit)
			m_GizmoType = m_GizmoType == ImGuizmo::OPERATION::TRANSLATE ? -1 : ImGuizmo::OPERATION::TRANSLATE;
	}
	ImGui::SameLine();
	if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(m_TexStore["RotateButton"]->GetRendererID()), ImVec2(32, 32),
	                       ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), RotateColorTint))
	{
		if (m_SceneState == SceneState::Edit)
			m_GizmoType = m_GizmoType == ImGuizmo::OPERATION::ROTATE ? -1 : ImGuizmo::OPERATION::ROTATE;
	}
	ImGui::SameLine();
	if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(m_TexStore["ScaleButton"]->GetRendererID()), ImVec2(32, 32),
	                       ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ScaleColorTint))
	{
		if (m_SceneState == SceneState::Edit)
			m_GizmoType = m_GizmoType == ImGuizmo::OPERATION::SCALE ? -1 : ImGuizmo::OPERATION::SCALE;
	}

	ImGui::SameLine();
	ImGui::Separator();
	ImGui::SameLine();

	ImGui::SameLine();
	if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(m_TexStore["ControllerGameButton"]->GetRendererID()),
	                       ImVec2(32, 32), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ControllerWASDTint))
	{
		if (m_SceneState == SceneState::Edit)
			scene->GetEntity().GetComponent<EditorCameraComponent>().Camera->SetControllerStyle(
				EditorCamera::ControllerStyle::Game);
	}
	ImGui::SameLine();
	if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(m_TexStore["ControllerMayaButton"]->GetRendererID()),
	                       ImVec2(32, 32), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ControllerMayaTint))
	{
		if (m_SceneState == SceneState::Edit)
			scene->GetEntity().GetComponent<EditorCameraComponent>().Camera->SetControllerStyle(
				EditorCamera::ControllerStyle::Maya);
	}

	ImGui::End();
	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar(3);
}

bool EditorLayer::OnEntitySelected(Entity entity)
{
	auto activeScene = GetActiveScene();
	if (activeScene == m_EditorScene || activeScene == m_RuntimeScene)
	{
		m_SelectedEntity = entity;
		m_EntityPanel->SetSelectedEntity(entity);
		m_ScenePanel->SetSelectedEntity(entity);
		activeScene->SetSelectedEntity(entity);
	}
	return false;
}

bool EditorLayer::OnEntityUnselected(Entity entity)
{
	auto activeScene = GetActiveScene();
	if (activeScene == m_EditorScene || activeScene == m_RuntimeScene)
	{
		m_SelectedEntity = {};
		m_EntityPanel->SetSelectedEntity({});
		m_ScenePanel->SetSelectedEntity({});
		activeScene->SetSelectedEntity({});
	}
	return false;
}

bool EditorLayer::OnEntityDeleted(Entity entity)
{
	if (m_SelectedEntity == entity)
	{
		m_EditorScene->DestroyEntity(m_SelectedEntity);
		OnEntityUnselected(m_SelectedEntity);
	}
	return false;
}

bool EditorLayer::OnEntityCopied(Entity entity)
{
	if (m_SelectedEntity == entity)
	{
		const Entity copy = m_SelectedEntity.Copy();
		OnEntitySelected(copy);
	}
	return false;
}

bool EditorLayer::OnNewModelSpaceView(Entity entity)
{
	const auto& tag = entity.GetComponent<TagComponent>().Tag;
	auto newScene = CreateShared<ModelSpaceScene>(entity);
	auto newViewportPane = CreateShared<ViewportPane>(tag, newScene->GetTarget());
	auto& emplacedPair = m_ModelSpaceSceneViews.emplace_back(newScene, newViewportPane);
	m_DockedModelSpaceScene.push_back(false);

	const auto postRenderFn = [this, emplacedPair]
	{
		const auto& scene = emplacedPair.first;
		const auto& viewportPane = emplacedPair.second;
		Entity modelEntity = scene->GetModelEntity();

		if (m_SceneState == SceneState::Edit && m_GizmoType != -1 && GetActiveScene() == scene && modelEntity)
		{
			const auto viewportSize = viewportPane->GetViewportSize();
			const auto& editorCamera = scene->GetEntity().GetComponent<EditorCameraComponent>().Camera;

			const bool snap = Keyboard::IsPressed(KeyCode::LControl);
			auto& entityTransform = modelEntity.Transform();
			const float snapValue = GetSnapValue();
			float snapValues[3] = {snapValue, snapValue, snapValue};

			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, viewportSize.x, viewportSize.y);

			if (m_SelectionMode == SelectionMode::Entity)
			{
				ImGuizmo::Manipulate(glm::value_ptr(editorCamera->GetViewMatrix()),
				                     glm::value_ptr(editorCamera->GetProjectionMatrix()),
				                     static_cast<ImGuizmo::OPERATION>(m_GizmoType), ImGuizmo::LOCAL,
				                     glm::value_ptr(entityTransform), nullptr, snap ? snapValues : nullptr);
			}
			else
			{
				glm::mat4 transformBase = entityTransform * modelEntity.GetComponent<TransformComponent>().Transform;
				ImGuizmo::Manipulate(glm::value_ptr(editorCamera->GetViewMatrix()),
				                     glm::value_ptr(editorCamera->GetProjectionMatrix()),
				                     static_cast<ImGuizmo::OPERATION>(m_GizmoType), ImGuizmo::LOCAL,
				                     glm::value_ptr(transformBase), nullptr, snap ? snapValues : nullptr);

				modelEntity.GetComponent<TransformComponent>().Transform = glm::inverse(entityTransform) *
					transformBase;
			}
		}
	};

	emplacedPair.second->FinishedRender += [postRenderFn]
	{
		postRenderFn();
		return false;
	};

	return false;
}

Ray EditorLayer::CastMouseRay() const
{
	const auto& focusedViewport = GetActiveViewportPane();
	const auto MousePosition = focusedViewport->GetMousePosition();
	if (MousePosition.x > -1.0f && MousePosition.x < 1.0f && MousePosition.y > -1.0f && MousePosition.y < 1.0f)
	{
		auto [origin, direction] = CastRay(MousePosition.x, MousePosition.y);
		return Ray(origin, direction);
	}
	return Ray::Zero();
}
}
