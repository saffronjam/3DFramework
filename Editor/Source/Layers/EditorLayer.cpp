#include "SaffronPCH.h"

#include "EditorLayer.h"

namespace Se
{
EditorLayer::EditorLayer(const Shared<Project>& project) :
	_project(Move(project)),
	_mainViewportPane(Shared<ViewportPane>::Create("Main Viewport")),
	_lastFocusedScene(_editorScene),
	_cachedActiveScene(_editorScene),
	_sceneState(SceneState::Edit)
{
	App::Instance().GetWindow().SetAntiAliasing(AntiAliasing::Sample16);
}

void EditorLayer::OnAttach(Shared<BatchLoader>& loader)
{
	loader->Submit([this]
	{
		Gui::SetStyle(static_cast<Gui::Style>(_style));
	}, "Initializing GUI Style");

	loader->Submit([this]
	{
		_texStore["Checkerboard"] = Texture2D::Create("Editor/Checkerboard.tga");
		_texStore["Checkerboard2"] = Texture2D::Create("Editor/Checkerboard.tga");
		_texStore["PlayButton"] = Texture2D::Create("Editor/PlayButton.png");
		_texStore["PauseButton"] = Texture2D::Create("Editor/PauseButton.png");
		_texStore["StopButton"] = Texture2D::Create("Editor/StopButton.png");
		_texStore["TranslateButton"] = Texture2D::Create("Editor/Translate_w.png");
		_texStore["RotateButton"] = Texture2D::Create("Editor/Rotate_w.png");
		_texStore["ScaleButton"] = Texture2D::Create("Editor/Scale_w.png");
		_texStore["ControllerGameButton"] = Texture2D::Create("Editor/ControllerGame_w.png");
		_texStore["ControllerMayaButton"] = Texture2D::Create("Editor/ControllerMaya_w.png");
	}, "Loading Editor Textures");


	loader->Submit([this]
	{
		if (_project->IsValid())
		{
			_assetPanel = Shared<AssetPanel>::Create(_project->GetProjectFolderpath().string() + "/Assets/Meshes");
			_scriptPanel = Shared<ScriptPanel>::Create(_project->GetProjectFolderpath().string() + "/Src");
		}
		else
		{
			_assetPanel = Shared<AssetPanel>::Create();
			_scriptPanel = Shared<ScriptPanel>::Create();
		}
		_entityPanel = Shared<EntityComponentsPanel>::Create(_editorScene);
		_scenePanel = Shared<SceneHierarchyPanel>::Create(_editorScene);
	}, "Initializing Editor Panels");

	loader->Submit([this]
	{
		OnProjectChange(_project);
	}, "Loading Default Scene");

	loader->Submit([this]
	{
		_assetScriptRunHandle = Run::Periodically([this]
		{
			_assetPanel->SyncAssetPaths();
			_scriptPanel->SyncScriptPaths();
		}, Time(1.0f));
	}, "Setting Up Periodic Callbacks");

	loader->Submit([this]
	{
		const auto postRenderFn = [this]()
		{
			const auto& scene = _editorScene;
			const auto& viewportPane = _mainViewportPane;
			Entity modelEntity = _selectedEntity;

			if (_sceneState == SceneState::Edit && _gizmoType != -1 && GetActiveScene() == scene && modelEntity)
			{
				const auto viewportSize = viewportPane->GetViewportSize();
				const auto& editorCamera = scene->GetEntity().GetComponent<EditorCameraComponent>().Camera;

				const bool snap = Keyboard::IsPressed(KeyCode::LControl);
				auto& entityTransform = modelEntity.Transform();
				const float snapValue = GetSnapValue();
				float snapValues[3] = {snapValue, snapValue, snapValue};

				ImGuizmo::SetDrawlist();
				ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, viewportSize.x, viewportSize.y);				

				if (_selectionMode == SelectionMode::Entity)
				{
					ImGuizmo::Manipulate(glm::value_ptr(editorCamera->GetViewMatrix()),
					                     glm::value_ptr(editorCamera->GetProjectionMatrix()),
					                     static_cast<ImGuizmo::OPERATION>(_gizmoType), ImGuizmo::WORLD,
					                     glm::value_ptr(entityTransform), nullptr, snap ? snapValues : nullptr);
				}
				else
				{
					glm::mat4 transformBase = entityTransform * modelEntity.GetComponent<TransformComponent>().
					                                                        Transform;
					ImGuizmo::Manipulate(glm::value_ptr(editorCamera->GetViewMatrix()),
					                     glm::value_ptr(editorCamera->GetProjectionMatrix()),
					                     static_cast<ImGuizmo::OPERATION>(_gizmoType), ImGuizmo::WORLD,
					                     glm::value_ptr(transformBase), nullptr, snap ? snapValues : nullptr);

					modelEntity.GetComponent<TransformComponent>().Transform = glm::inverse(entityTransform) *
						transformBase;
				}
			}
		};

		_mainViewportPane->FinishedRender += [postRenderFn]
		{
			postRenderFn();
			return false;
		};

		_scenePanel->EntityDeleted += SE_BIND_EVENT_FN(OnEntityDeleted);
		_scenePanel->EntityCopied += SE_BIND_EVENT_FN(OnEntityCopied);
		_scenePanel->ViewInModelSpace += SE_BIND_EVENT_FN(EditorLayer::OnNewModelSpaceView);
		_scenePanel->NewSelection += [this](Entity entity)
		{
			OnEntityUnselected(_selectedEntity);
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
	if (_sceneState == SceneState::Play)
	{
		OnStop();
	}
	Run::Remove(_assetScriptRunHandle);
}

void EditorLayer::OnUpdate()
{
	const auto activeScene = GetActiveScene();
	if (activeScene != _cachedActiveScene)
	{
		OnSceneChange(activeScene, {});
	}

	switch (_sceneState)
	{
	case SceneState::Edit:
	{
		_mainViewportPane->IsFocused() ? _editorScene->EnableCamera() : _editorScene->DisableCamera();

		_editorScene->OnUpdate();
		_editorScene->OnRender();
		break;
	}
	case SceneState::Play:
	{
		_runtimeScene->OnUpdate();
		_runtimeScene->OnRender();
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
	_editorTerminal.OnGuiRender();
	_assetPanel->OnGuiRender();
	_entityPanel->OnGuiRender();
	_scriptPanel->OnGuiRender();
	_scenePanel->OnGuiRender(_scriptPanel);
	GetActiveScene()->OnGuiRender();
	_mainViewportPane->OnGuiRender();

	ImGui::End();

	// Correct scene viewport size after rendering viewport panes
	{
		const auto viewportSize = _mainViewportPane->GetViewportSize();
		if (_sceneState == SceneState::Play)
		{
			_runtimeScene->SetViewportSize(viewportSize.x, viewportSize.y);
		}
		_editorScene->SetViewportSize(viewportSize.x, viewportSize.y);
	}
}

bool EditorLayer::OnKeyboardPress(const KeyPressedEvent& event)
{
	if (_sceneState == SceneState::Edit && _mainViewportPane->IsFocused())
	{
		switch (event.GetKey())
		{
		case KeyCode::Delete: if (_selectedEntity)
			{
				OnEntityDeleted(_selectedEntity);
			}
			break;
		default: break;
		}

		if (Keyboard::IsPressed(KeyCode::LControl))
		{
			switch (event.GetKey())
			{
			case KeyCode::C: if (_selectedEntity)
				{
					OnEntityCopied(_selectedEntity);
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
		Keyboard::IsPressed(KeyCode::LAlt) && !ImGuizmo::IsOver() && _sceneState != SceneState::Play)
	{
		const auto MousePosition = activeViewport->GetMousePosition();
		if (MousePosition.x > -1.0f && MousePosition.x < 1.0f && MousePosition.y > -1.0f && MousePosition.y < 1.0f)
		{
			auto rayResult = CastRay(MousePosition.x, MousePosition.y);

			if (_selectedEntity)
			{
				OnEntityUnselected(_selectedEntity);
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
				for (uint i = 0; i < submeshes.size(); i++)
				{
					const auto& submesh = submeshes[i];
					Ray ray = {
						glm::inverse(entity.Transform() * mesh->GetLocalTransform() * submesh.Transform) * Vector4(
							rayResult.first, 1.0f),
						glm::inverse(
							Matrix3(entity.Transform()) * Matrix3(submesh.Transform) * Matrix3(
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
				const Entity entity = {entityHandle, _editorScene.Raw()};
				auto mesh = entity.GetComponent<MeshComponent>().Mesh;
				if (mesh)
				{
					raycast(entity, mesh);
				}
			}
			auto camereaEntities = GetActiveScene()->GetEntityRegistry().view<CameraComponent>();
			for (auto entityHandle : camereaEntities)
			{
				const Entity entity = {entityHandle, _editorScene.Raw()};
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
	if (_sceneState == SceneState::Edit)
	{
		for (const auto& filepath : event.GetFilepaths())
		{
			if (Scene::IsValidFilepath(filepath))
			{
				Run::Later([filepath, this]
				{
					const auto& transformedFilepath = _project->AddScene(filepath);
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
		Shared<EditorScene> newScene = Shared<EditorScene>::Create(sceneName);

		// Default construct environment and light
		// TODO: Prompt user with templates instead?
		const Light light = {{-0.5f, -0.5f, 1.0f}, {1.0f, 1.0f, 1.0f}, 1.0f};
		newScene->SetLight(light);

		_editorScene = newScene;
		_lastFocusedScene = _editorScene;

		SaveActiveScene();
		OnSceneChange(_editorScene, {});

		_project->AddScene(filepath);
		_project->AddCachedScene(newScene);
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
		const auto& retFilepath = _project->AddScene(Move(filepath));
		LoadProjectScene(filepath);
	}
}

void EditorLayer::PromptOpenProject()
{
	const Filepath filepath = FileIOManager::OpenFile({"Saffron Scene (*.spr)", {"*.spr"}});
	if (Project::IsValidProjectFilepath(filepath))
	{
		auto newProject = Shared<Project>::Create(filepath);
		if (newProject->IsValid())
		{
			OnProjectChange(newProject);
		}
		else
		{
			Log::Warn("Failed to deserialize project");
		}
	}
}

void EditorLayer::SaveActiveProject() const
{
	for (const auto& cachedScene : _project->GetSceneCache())
	{
		cachedScene->Save();
	}
}

void EditorLayer::SaveActiveScene() const
{
	_editorScene->Save();
}

void EditorLayer::LoadProjectScene(const Filepath& filepath)
{
	if (!_project->IsValidSceneFilepath(filepath))
	{
		return;
	}
	const auto fullSceneFilepath = _project->GetFullSceneFilepath(filepath);

	Shared<EditorScene> newScene(nullptr);

	for (const auto& cachedScene : _project->GetSceneCache())
	{
		if (cachedScene->GetFilepath() == filepath)
		{
			newScene = cachedScene;
		}
	}

	if (!newScene)
	{
		newScene = Shared<EditorScene>::Create(fullSceneFilepath);
		_project->AddCachedScene(newScene);
	}

	_editorScene = newScene;
	_lastFocusedScene = _editorScene;

	OnEntityUnselected(_selectedEntity);
	OnSceneChange(_editorScene, {});
}

void EditorLayer::OnSceneChange(Shared<Scene> scene, Entity selection)
{
	ScriptEngine::OnSceneChange(scene);
	_entityPanel->SetContext(scene);
	_entityPanel->SetSelectedEntity(selection ? selection : scene->GetSelectedEntity());
	_scenePanel->SetContext(scene);
	_scenePanel->SetSelectedEntity(selection ? selection : scene->GetSelectedEntity());
	_selectedEntity = selection ? selection : scene->GetSelectedEntity();
	_cachedActiveScene = scene;
	_lastFocusedScene = scene;
}

void EditorLayer::OnProjectChange(const Shared<Project>& project)
{
	Debug::Assert(project->IsValid());
	ScriptEngine::OnProjectChange(project);
	_project = project;
	_assetPanel->SetAssetFolderpath(_project->GetProjectFolderpath().string() + "Assets/Meshes");
	_scriptPanel->SetAssetFolderpath(_project->GetProjectFolderpath().string() + "Source");

	LoadProjectScene(_project->GetSceneFilepaths().front());
}

void EditorLayer::OnWantProjectSelector()
{
	WantProjectSelector.Invoke();
}

void EditorLayer::OnPlay()
{
	_sceneState = SceneState::Play;

	if (_reloadScriptOnPlay && _project->IsValid())
	{
		ScriptEngine::ReloadAssembly(_project->GetProjectFilepath().stem().string());
	}
	_runtimeScene = Shared<RuntimeScene>::Create(_editorScene);
	_runtimeScene->OnStart();
	_gizmoType = -1;

	// Swap selected entity to the copied entity in the runtime scene
	Entity transfer = {};
	if (_selectedEntity)
	{
		const auto& entityMap = _runtimeScene->GetEntityMap();
		const UUID selectedEntityUUID = _selectedEntity.GetUUID();
		if (entityMap.find(selectedEntityUUID) != entityMap.end()) transfer = entityMap.at(selectedEntityUUID);
	}
	OnSceneChange(_runtimeScene, transfer);
}

void EditorLayer::OnStop()
{
	_sceneState = SceneState::Edit;
	_runtimeScene->OnStop();

	// Swap selected entity to the copied entity in the editor scene
	Entity transfer = {};
	if (_selectedEntity)
	{
		const auto& entityMap = _editorScene->GetEntityMap();
		const Se::UUID selectedEntityUUID = _selectedEntity.GetUUID();
		if (entityMap.find(selectedEntityUUID) != entityMap.end())
		{
			transfer = entityMap.at(selectedEntityUUID);
		}
	}
	OnSceneChange(_editorScene, transfer);

	// Unload runtime scene
	_runtimeScene = nullptr;
}

float EditorLayer::GetSnapValue() const
{
	switch (_gizmoType)
	{
	case ImGuizmo::OPERATION::TRANSLATE: return 0.5f;
	case ImGuizmo::OPERATION::ROTATE: return 45.0f;
	case ImGuizmo::OPERATION::SCALE: return 0.5f;
	default: return 0.0f;
	}
}

Shared<ViewportPane> EditorLayer::GetActiveViewportPane() const
{
	if (_sceneState == SceneState::Play)
	{
		return _mainViewportPane;
	}

	const auto activeScene = GetActiveScene();
	if (activeScene == _editorScene)
	{
		return _mainViewportPane;
	}
	Log::Warn("Detected a scene without a viewport pane. Scene name: {}", activeScene->GetName());
	return _mainViewportPane;
}

Shared<Scene> EditorLayer::GetActiveScene() const
{
	if (_sceneState == SceneState::Play)
	{
		_lastFocusedScene = _runtimeScene;
		return _lastFocusedScene;
	}

	if (_mainViewportPane->IsFocused())
	{
		_lastFocusedScene = _editorScene;
		return _lastFocusedScene;
	}

	return _lastFocusedScene;
}


Pair<Vector3, Vector3> EditorLayer::CastRay(float mx, float my) const
{
	const Vector4 mouseClipPos = {mx, my, -1.0f, 1.0f};

	auto scene = GetActiveScene();
	if (scene->GetEntity().HasComponent<EditorCameraComponent>())
	{
		const auto& editorCamera = scene->GetEntity().GetComponent<EditorCameraComponent>().Camera;

		const auto inverseProj = glm::inverse(editorCamera->GetProjectionMatrix());
		const auto inverseView = glm::inverse(glm::mat3(editorCamera->GetViewMatrix()));

		const Vector4 ray = inverseProj * mouseClipPos;
		const Vector3 rayPos = editorCamera->GetPosition();
		const Vector3 rayDir = inverseView * Vector3(ray);

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
			if (_sceneState == SceneState::Play)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}
			if (ImGui::MenuItem("Reload C# Assembly") && _project->IsValid())
			{
				ScriptEngine::ReloadAssembly(_project->GetProjectFilepath().stem().string());
			}
			if (_sceneState == SceneState::Play)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}

			ImGui::MenuItem("Reload assembly on play", nullptr, &_reloadScriptOnPlay);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Style"))
		{
			ImGui::RadioButton("DarkColorful", &_style, 0);
			ImGui::RadioButton("Light", &_style, 1);
			ImGui::EndMenu();
		}
		Gui::SetStyle(static_cast<Gui::Style>(_style));

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


	Shared<Texture> ButtonTexture = _sceneState == SceneState::Edit
		                                ? _texStore["PlayButton"]
		                                : _texStore["StopButton"];
	if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(ButtonTexture->GetRendererID()), ImVec2(25, 32), ImVec2(0, 0),
	                       ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ClearWhite))
	{
		_sceneState == SceneState::Edit ? OnPlay() : OnStop();
	}

	ImGui::SameLine();
	ImGui::Separator();
	ImGui::SameLine();

	ImVec4 TranslateColorTint = _gizmoType == ImGuizmo::OPERATION::TRANSLATE ? ImGuiBlue : ClearWhite;
	ImVec4 RotateColorTint = _gizmoType == ImGuizmo::OPERATION::ROTATE ? ImGuiBlue : ClearWhite;
	ImVec4 ScaleColorTint = _gizmoType == ImGuizmo::OPERATION::SCALE ? ImGuiBlue : ClearWhite;
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


	if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(_texStore["TranslateButton"]->GetRendererID()),
	                       ImVec2(32, 32), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), TranslateColorTint))
	{
		if (_sceneState == SceneState::Edit)
			_gizmoType = _gizmoType == ImGuizmo::OPERATION::TRANSLATE ? -1 : ImGuizmo::OPERATION::TRANSLATE;
	}
	ImGui::SameLine();
	if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(_texStore["RotateButton"]->GetRendererID()), ImVec2(32, 32),
	                       ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), RotateColorTint))
	{
		if (_sceneState == SceneState::Edit)
			_gizmoType = _gizmoType == ImGuizmo::OPERATION::ROTATE ? -1 : ImGuizmo::OPERATION::ROTATE;
	}
	ImGui::SameLine();
	if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(_texStore["ScaleButton"]->GetRendererID()), ImVec2(32, 32),
	                       ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ScaleColorTint))
	{
		if (_sceneState == SceneState::Edit)
			_gizmoType = _gizmoType == ImGuizmo::OPERATION::SCALE ? -1 : ImGuizmo::OPERATION::SCALE;
	}

	ImGui::SameLine();
	ImGui::Separator();
	ImGui::SameLine();

	ImGui::SameLine();
	if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(_texStore["ControllerGameButton"]->GetRendererID()),
	                       ImVec2(32, 32), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ControllerWASDTint))
	{
		if (_sceneState == SceneState::Edit)
			scene->GetEntity().GetComponent<EditorCameraComponent>().Camera->SetControllerStyle(
				EditorCamera::ControllerStyle::Game);
	}
	ImGui::SameLine();
	if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(_texStore["ControllerMayaButton"]->GetRendererID()),
	                       ImVec2(32, 32), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ControllerMayaTint))
	{
		if (_sceneState == SceneState::Edit)
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
	if (activeScene == _editorScene || activeScene == _runtimeScene)
	{
		_selectedEntity = entity;
		_entityPanel->SetSelectedEntity(entity);
		_scenePanel->SetSelectedEntity(entity);
		activeScene->SetSelectedEntity(entity);
	}
	return false;
}

bool EditorLayer::OnEntityUnselected(Entity entity)
{
	auto activeScene = GetActiveScene();
	if (activeScene == _editorScene || activeScene == _runtimeScene)
	{
		_selectedEntity = {};
		_entityPanel->SetSelectedEntity({});
		_scenePanel->SetSelectedEntity({});
		activeScene->SetSelectedEntity({});
	}
	return false;
}

bool EditorLayer::OnEntityDeleted(Entity entity)
{
	if (_selectedEntity == entity)
	{
		_editorScene->DestroyEntity(_selectedEntity);
		OnEntityUnselected(_selectedEntity);
	}
	return false;
}

bool EditorLayer::OnEntityCopied(Entity entity)
{
	if (_selectedEntity == entity)
	{
		const Entity copy = _selectedEntity.Copy();
		OnEntitySelected(copy);
	}
	return false;
}

bool EditorLayer::OnNewModelSpaceView(Entity entity)
{
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
