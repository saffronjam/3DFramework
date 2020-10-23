#include "EditorLayer.h"

#include <filesystem>

#include "Saffron/Core/Misc.h"
#include "Saffron/Core/FileIOManager.h"
#include "Saffron/Editor/ViewportPane.h"
#include "Saffron/Editor/EditorTerminal.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Input/Input.h"
#include "Saffron/Renderer/Renderer2D.h"
#include "Saffron/Script/ScriptEngine.h"
#include "Saffron/System/Macros.h"


namespace Se
{

EditorLayer::EditorLayer()
	:
	m_Style(static_cast<int>(Gui::Style::Dark)),
	m_EditorCamera(m_MainViewport, glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f)),
	m_MainViewport("MainRenderTarget"),
	m_MiniViewport("MiniRenderTarget")
{
	EditorTerminal::Init();
	Gui::Init();

	SceneRenderer::AddRenderTarget("MiniRenderTarget", 200, 100);
	SceneRenderer::DisableRenderTarget("MiniRenderTarget");
}

void EditorLayer::OnAttach()
{
	Gui::SetStyle(static_cast<Gui::Style>(m_Style));

	// Editor
	m_TexStore["Checkerboard"] = Texture2D::Create("Assets/Editor/Checkerboard.tga");
	m_TexStore["PlayButton"] = Texture2D::Create("Assets/Editor/PlayButton.png");
	m_TexStore["PauseButton"] = Texture2D::Create("Assets/Editor/PauseButton.png");
	m_TexStore["StopButton"] = Texture2D::Create("Assets/Editor/StopButton.png");
	m_TexStore["TranslateButton"] = Texture2D::Create("Assets/Editor/Translate_w.png");
	m_TexStore["RotateButton"] = Texture2D::Create("Assets/Editor/Rotate_w.png");
	m_TexStore["ScaleButton"] = Texture2D::Create("Assets/Editor/Scale_w.png");
	m_TexStore["ControllerGameButton"] = Texture2D::Create("Assets/Editor/ControllerGame_w.png");
	m_TexStore["ControllerMayaButton"] = Texture2D::Create("Assets/Editor/ControllerMaya_w.png");

	m_AssetPanel = Ref<AssetPanel>::Create("../ExampleApp/Assets/Meshes");
	m_EntityPanel = Ref<EntityPanel>::Create(m_EditorScene);
	m_ScriptPanel = Ref<ScriptPanel>::Create("../ExampleApp/Src");

	m_EntityPanel->SetSelectionChangedCallback([this](Entity entity) { SelectEntity(entity); });
	m_EntityPanel->SetEntityDeletedCallback([this](Entity entity) { OnEntityDeleted(entity); });

	LoadNewScene("Assets/Scenes/Levels/Physics2D-Game.ssc");

	ScriptEngine::SetSceneContext(m_EditorScene);


	m_MainViewport.SetPostRenderCallback([this]()
										 {
											 // Gizmos
											 if ( m_GizmoType != -1 && m_SelectedEntity )
											 {
												 const auto viewportSize = m_MainViewport.GetViewportSize();
												 ImGuizmo::SetDrawlist();
												 ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, viewportSize.x, viewportSize.y);

												 const bool snap = Input::IsKeyPressed(SE_KEY_LEFT_CONTROL);

												 auto &entityTransform = m_SelectedEntity.Transform();
												 const float snapValue = GetSnapValue();
												 float snapValues[3] = { snapValue, snapValue, snapValue };
												 if ( m_SelectionMode == SelectionMode::Entity )
												 {
													 ImGuizmo::Manipulate(glm::value_ptr(m_EditorCamera.GetViewMatrix()),
																		  glm::value_ptr(m_EditorCamera.GetProjectionMatrix()),
																		  static_cast<ImGuizmo::OPERATION>(m_GizmoType),
																		  ImGuizmo::LOCAL,
																		  glm::value_ptr(entityTransform),
																		  nullptr,
																		  snap ? snapValues : nullptr);
												 }
												 else
												 {
													 glm::mat4 transformBase = entityTransform * m_SelectedEntity.GetComponent<TransformComponent>().Transform;
													 ImGuizmo::Manipulate(glm::value_ptr(m_EditorCamera.GetViewMatrix()),
																		  glm::value_ptr(m_EditorCamera.GetProjectionMatrix()),
																		  static_cast<ImGuizmo::OPERATION>(m_GizmoType),
																		  ImGuizmo::LOCAL,
																		  glm::value_ptr(transformBase),
																		  nullptr,
																		  snap ? snapValues : nullptr);

													 m_SelectedEntity.GetComponent<TransformComponent>().Transform = glm::inverse(entityTransform) * transformBase;
												 }
											 }
										 });
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnSceneChange()
{
	if ( m_SceneState == SceneState::Play )
	{
		OnSceneStop();
	}
	m_EditorScene->SetSelectedEntity({});
	m_SelectedEntity = {};
}

void EditorLayer::OnScenePlay()
{
	m_SceneState = SceneState::Play;

	if ( m_ReloadScriptOnPlay )
		ScriptEngine::ReloadAssembly("Assets/Scripts/ExampleApp.dll");

	m_RuntimeScene = Ref<Scene>::Create("Runtime Scene");
	m_EditorScene->CopyTo(m_RuntimeScene);

	m_RuntimeScene->OnRuntimeStart();
	m_EntityPanel->SetContext(m_RuntimeScene);

	m_GizmoType = -1;

	// Swap selected entity to the copied entity in the runtime scene
	if ( m_SelectedEntity )
	{
		const auto &entityMap = m_RuntimeScene->GetEntityMap();
		const UUID selectedEntityUUID = m_SelectedEntity.GetUUID();
		if ( entityMap.find(selectedEntityUUID) != entityMap.end() )
			m_SelectedEntity = entityMap.at(selectedEntityUUID);
	}
}

void EditorLayer::OnSceneStop()
{
	m_RuntimeScene->OnRuntimeStop();
	m_SceneState = SceneState::Edit;

	ScriptEngine::SetSceneContext(m_EditorScene);
	m_EntityPanel->SetContext(m_EditorScene);

	// Swap selected entity to the copied entity in the editor scene
	if ( m_SelectedEntity )
	{
		const auto &entityMap = m_EditorScene->GetEntityMap();
		const UUID selectedEntityUUID = m_SelectedEntity.GetUUID();
		if ( entityMap.find(selectedEntityUUID) != entityMap.end() )
			m_SelectedEntity = entityMap.at(selectedEntityUUID);
	}
	m_EditorScene->SetSelectedEntity(m_SelectedEntity);

	// Unload runtime scene
	m_RuntimeScene = nullptr;
}

void EditorLayer::UpdateWindowTitle(const std::string &sceneName)
{
	const std::string title = sceneName + " - SaffronBun - " + Application::GetPlatformName() + " (" + Application::GetConfigurationName() + ")";
	Application::Get().GetWindow().SetTitle(title);
}

float EditorLayer::GetSnapValue() const
{
	switch ( m_GizmoType )
	{
	case  ImGuizmo::OPERATION::TRANSLATE:	return 0.5f;
	case  ImGuizmo::OPERATION::ROTATE:		return 45.0f;
	case  ImGuizmo::OPERATION::SCALE:		return 0.5f;
	default:								return 0.0f;
	}
}

void EditorLayer::OnUpdate()
{
	const Time ts = GlobalTimer::GetStep();

	switch ( m_SceneState )
	{
	case SceneState::Edit:
	{
		//if (m_ViewportPanelFocused)
		m_EditorCamera.OnUpdate(ts);

		m_EditorScene->OnRenderEditor(ts, m_EditorCamera);

		if ( false )
		{
			Renderer::BeginRenderPass(SceneRenderer::GetFinalRenderPass("MainRenderTarget"), false);
			const auto viewProj = m_EditorCamera.GetViewProjection();
			Renderer2D::BeginScene(viewProj, false);
			Renderer::DrawAABB(AABB{ glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3{ 100.0f, 100.0f, 100.0f } }, glm::mat4(1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
			Renderer2D::DrawLine(glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3{ 100.0f, 100.0f, 100.0f }, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			Renderer2D::EndScene();
			Renderer::EndRenderPass();
		}

		if ( m_SelectedEntity )
		{
			if ( m_SelectedEntity.HasComponent<BoxCollider2DComponent>() )
			{
				const auto &size = m_SelectedEntity.GetComponent<BoxCollider2DComponent>().Size;
				auto [translation, rotationQuat, scale] = Misc::GetTransformDecomposition(m_SelectedEntity.GetComponent<TransformComponent>().Transform);
				const glm::vec3 rotation = glm::eulerAngles(rotationQuat);

				Renderer::BeginRenderPass(SceneRenderer::GetFinalRenderPass("MainRenderTarget"), false);
				const auto viewProj = m_EditorCamera.GetViewProjection();
				Renderer2D::BeginScene(viewProj, false);
				Renderer2D::DrawRotatedQuad({ translation.x, translation.y }, size * 2.0f, glm::degrees(rotation.z), { 1.0f, 0.0f, 1.0f, 1.0f });
				Renderer2D::EndScene();
				Renderer::EndRenderPass();
			}
		}

		break;
	}
	case SceneState::Play:
	{
		m_RuntimeScene->OnUpdate(ts);
		m_RuntimeScene->OnRenderRuntime(ts);
		break;
	}
	case SceneState::Pause:
	{
		if ( m_MainViewport.IsFocused() )
			m_EditorCamera.OnUpdate(ts);

		m_RuntimeScene->OnRenderRuntime(ts);
		break;
	}
	}

	if ( m_SelectedEntity )
	{
		if ( m_SelectedEntity.HasComponent<CameraComponent>() && m_SelectedEntity.HasComponent<TransformComponent>() )
		{
			Ref<SceneCamera> camera = m_SelectedEntity.GetComponent<CameraComponent>().Camera;

			const auto viewportSize = m_MiniViewport.GetViewportSize();
			camera->SetViewportSize(static_cast<Uint32>(viewportSize.x), static_cast<Uint32>(viewportSize.y));
			const glm::mat4 cameraViewMatrix = glm::inverse(m_SelectedEntity.GetComponent<TransformComponent>().Transform);

			SceneRenderer::SetCameraData("MiniRenderTarget", { camera.Raw(), cameraViewMatrix });
			SceneRenderer::EnableRenderTarget("MiniRenderTarget");
		}
	}
}

void EditorLayer::SelectEntity(Entity entity)
{
	OnUnselected(m_SelectedEntity);
	OnSelected(entity);
}

void EditorLayer::NewScenePrompt()
{
	const std::filesystem::path filepath = FileIOManager::SaveFile({ "Saffron Scene (*.ssc)", {"*.ssc"} });

	if ( !filepath.empty() )
	{
		std::string sceneName = filepath.stem().string();
		Ref<Scene> newScene = Ref<Scene>::Create(sceneName);

		// Default construct environment and light
		// TODO: Prompt user with templates instead?
		newScene->SetEnvironment(Scene::Environment::Load("Assets/Env/birchwood_4k.hdr"));
		const Scene::Light light = { {-0.5f, -0.5f, 1.0f}, {1.0f, 1.0f, 1.0f}, 1.0f };
		newScene->SetLight(light);

		m_EditorScene = newScene;
		UpdateWindowTitle(sceneName);
		m_EntityPanel->SetContext(m_EditorScene);
		ScriptEngine::SetSceneContext(m_EditorScene);

		m_SceneFilePath = filepath;

		SaveActiveScene();
		OnSceneChange();
	}
}

void EditorLayer::OpenScenePrompt()
{
	const std::filesystem::path filepath = FileIOManager::OpenFile({ "Saffron Scene (*.ssc)", {"*.ssc"} });
	if ( !filepath.empty() )
	{
		LoadNewScene(filepath.string());
	}
}

void EditorLayer::SaveSceneAsPrompt()
{
	const std::filesystem::path filepath = FileIOManager::SaveFile({ "Saffron Scene (*.ssc)", {"*.ssc"} });
	if ( !filepath.empty() )
	{
		m_SceneFilePath = filepath;
		SaveActiveScene();
		UpdateWindowTitle(m_EditorScene->GetName());

	}
}

void EditorLayer::SaveActiveScene() const
{
	SceneSerializer serializer(m_EditorScene);
	serializer.Serialize(m_SceneFilePath.string());
}

void EditorLayer::LoadNewScene(const std::string &filepath)
{
	if ( filepath != m_SceneFilePath.string() )
	{
		const Ref<Scene> newScene = Ref<Scene>::Create("Editor scene");
		SceneSerializer serializer(newScene);
		if ( !serializer.Deserialize(filepath) )
		{
			SE_WARN("Failed to load scene! Filepath: {0}", filepath);
			return;
		}

		m_EditorScene = newScene;

		UpdateWindowTitle(m_EditorScene->GetName());
		m_EntityPanel->SetContext(m_EditorScene);
		ScriptEngine::SetSceneContext(m_EditorScene);

		OnUnselected(m_SelectedEntity);

		m_SceneFilePath = filepath;

		SaveActiveScene();
		OnSceneChange();
	}
	else
	{
		SE_INFO("Tried to load a scene that was already active");
	}
}

void EditorLayer::OnGuiRender()
{

	static bool p_open = true;

	static bool opt_fullscreen_persistent = true;
	static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None;
	const bool opt_fullscreen = opt_fullscreen_persistent;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if ( opt_fullscreen )
	{
		ImGuiViewport *viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	// When using ImGuiDockNodeFlags_PassthruDockspace, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
	//if (opt_flags & ImGuiDockNodeFlags_PassthruDockspace)
	//	window_flags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &p_open, window_flags);

	ImGui::ShowDemoWindow();

	ImGui::PopStyleVar();

	if ( opt_fullscreen )
		ImGui::PopStyleVar(2);

	// Dockspace
	ImGuiIO &io = ImGui::GetIO();
	if ( io.ConfigFlags & ImGuiConfigFlags_DockingEnable )
	{
		const ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
	}

	// TODO: Move to header?
	const auto ImGuiBlue = ImVec4{ 0.137f, 0.263f, 0.424f, 1.0f };
	const auto ClearWhite = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
	const auto FadedGrey = ImVec4{ 0.3f, 0.3f, 0.3f, 0.5f };

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 4));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.8f, 0.8f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
	ImGui::Begin("Toolbar");


	Ref<Texture> ButtonTexture = m_SceneState == SceneState::Edit ? m_TexStore["PlayButton"] : m_TexStore["StopButton"];
	if ( ImGui::ImageButton(reinterpret_cast<ImTextureID>(ButtonTexture->GetRendererID()), ImVec2(25, 32), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ClearWhite) )
	{
		m_SceneState == SceneState::Edit ? OnScenePlay() : OnSceneStop();
	}

	ImGui::SameLine();
	ImGui::Separator();
	ImGui::SameLine();

	ImVec4 TranslateColorTint = m_GizmoType == ImGuizmo::OPERATION::TRANSLATE ? ImGuiBlue : ClearWhite;
	ImVec4 RotateColorTint = m_GizmoType == ImGuizmo::OPERATION::ROTATE ? ImGuiBlue : ClearWhite;
	ImVec4 ScaleColorTint = m_GizmoType == ImGuizmo::OPERATION::SCALE ? ImGuiBlue : ClearWhite;
	ImVec4 ControllerWASDTint = m_EditorCamera.GetControllerStyle() == EditorCamera::ControllerStyle::Game ? ImGuiBlue : ClearWhite;
	ImVec4 ControllerMayaTint = m_EditorCamera.GetControllerStyle() == EditorCamera::ControllerStyle::Maya ? ImGuiBlue : ClearWhite;
	if ( m_SceneState == SceneState::Play )
	{
		TranslateColorTint = FadedGrey;
		RotateColorTint = FadedGrey;
		ScaleColorTint = FadedGrey;
		ControllerWASDTint = FadedGrey;
		ControllerMayaTint = FadedGrey;
	}


	if ( ImGui::ImageButton(reinterpret_cast<ImTextureID>(m_TexStore["TranslateButton"]->GetRendererID()), ImVec2(32, 32), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), TranslateColorTint) )
	{
		if ( m_SceneState == SceneState::Edit )
			m_GizmoType = m_GizmoType == ImGuizmo::OPERATION::TRANSLATE ? -1 : ImGuizmo::OPERATION::TRANSLATE;
	}
	ImGui::SameLine();
	if ( ImGui::ImageButton(reinterpret_cast<ImTextureID>(m_TexStore["RotateButton"]->GetRendererID()), ImVec2(32, 32), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), RotateColorTint) )
	{
		if ( m_SceneState == SceneState::Edit )
			m_GizmoType = m_GizmoType == ImGuizmo::OPERATION::ROTATE ? -1 : ImGuizmo::OPERATION::ROTATE;
	}
	ImGui::SameLine();
	if ( ImGui::ImageButton(reinterpret_cast<ImTextureID>(m_TexStore["ScaleButton"]->GetRendererID()), ImVec2(32, 32), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ScaleColorTint) )
	{
		if ( m_SceneState == SceneState::Edit )
			m_GizmoType = m_GizmoType == ImGuizmo::OPERATION::SCALE ? -1 : ImGuizmo::OPERATION::SCALE;
	}

	ImGui::SameLine();
	ImGui::Separator();
	ImGui::SameLine();

	ImGui::SameLine();
	if ( ImGui::ImageButton(reinterpret_cast<ImTextureID>(m_TexStore["ControllerGameButton"]->GetRendererID()), ImVec2(32, 32), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ControllerWASDTint) )
	{
		if ( m_SceneState == SceneState::Edit )
			m_EditorCamera.SetControllerStyle(EditorCamera::ControllerStyle::Game);
	}
	ImGui::SameLine();
	if ( ImGui::ImageButton(reinterpret_cast<ImTextureID>(m_TexStore["ControllerMayaButton"]->GetRendererID()), ImVec2(32, 32), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ControllerMayaTint) )
	{
		if ( m_SceneState == SceneState::Edit )
			m_EditorCamera.SetControllerStyle(EditorCamera::ControllerStyle::Maya);
	}

	ImGui::End();
	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar(3);


	if ( ImGui::BeginMenuBar() )
	{
		if ( ImGui::BeginMenu("File") )
		{
			if ( ImGui::MenuItem("New Scene", "Ctrl-N") )
				NewScenePrompt();
			if ( ImGui::MenuItem("Open Scene...", "Ctrl+O") )
				OpenScenePrompt();
			ImGui::Separator();
			if ( ImGui::MenuItem("Save Scene", "Ctrl+S") )
				SaveActiveScene();
			if ( ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S") )
				SaveSceneAsPrompt();

			ImGui::Separator();
			if ( ImGui::MenuItem("Exit") )
			{
				p_open = false;
				Application::Get().Exit();
			}
			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu("Script") )
		{
			if ( m_SceneState == SceneState::Play )
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}
			if ( ImGui::MenuItem("Reload C# Assembly") )
				ScriptEngine::ReloadAssembly("Assets/Scripts/ExampleApp.dll");
			if ( m_SceneState == SceneState::Play )
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}

			ImGui::MenuItem("Reload assembly on play", nullptr, &m_ReloadScriptOnPlay);
			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu("Style") )
		{
			ImGui::RadioButton("Dark", &m_Style, 0);
			ImGui::RadioButton("Light", &m_Style, 1);
			ImGui::EndMenu();
		}
		Gui::SetStyle(static_cast<Gui::Style>(m_Style));

		ImGui::EndMenuBar();
	}

	EditorTerminal::OnGuiRender();
	Renderer::OnGuiRender();
	ScriptEngine::OnGuiRender();
	Shader::OnGuiRender();
	m_MainViewport.OnGuiRender();
	m_MiniViewport.OnGuiRender();
	m_AssetPanel->OnGuiRender();
	m_EntityPanel->OnGuiRender(m_ScriptPanel);
	m_ScriptPanel->OnGuiRender();
	m_SceneState == SceneState::Edit ? m_EditorScene->OnGuiRender() : m_RuntimeScene->OnGuiRender();

	ImGui::End();

	// Correct viewport and camera matrices
	const auto mainViewportSize = m_MainViewport.GetViewportSize();
	m_EditorScene->SetViewportSize(static_cast<Uint32>(mainViewportSize.x), static_cast<Uint32>(mainViewportSize.y));
	if ( m_RuntimeScene )
		m_RuntimeScene->SetViewportSize(static_cast<Uint32>(mainViewportSize.x), static_cast<Uint32>(mainViewportSize.y));
	m_EditorCamera.SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), mainViewportSize.x, mainViewportSize.y, 0.1f, 10000.0f));
	m_EditorCamera.SetViewportSize(static_cast<Uint32>(mainViewportSize.x), static_cast<Uint32>(mainViewportSize.y));
}

void EditorLayer::OnEvent(const Event &event)
{
	if ( m_SceneState == SceneState::Edit )
	{
		if ( m_MainViewport.IsHovered() )
			m_EditorCamera.OnEvent(event);

		m_EditorScene->OnEvent(event);
	}
	else if ( m_SceneState == SceneState::Play )
	{
		m_RuntimeScene->OnEvent(event);
	}

	const EventDispatcher dispatcher(event);
	dispatcher.Try<KeyboardPressEvent>(SE_BIND_EVENT_FN(EditorLayer::OnKeyboardPressEvent));
	dispatcher.Try<MousePressEvent>(SE_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	dispatcher.Try<WindowDropFilesEvent>(SE_BIND_EVENT_FN(EditorLayer::OnWindowDropFiles));
}

bool EditorLayer::OnKeyboardPressEvent(const KeyboardPressEvent &event)
{
	if ( m_MainViewport.IsFocused() )
	{
		switch ( event.GetKey() )
		{
		case KeyCode::Delete:
			if ( m_SelectedEntity )
			{
				OnUnselected(m_SelectedEntity);
				m_EditorScene->DestroyEntity(m_SelectedEntity);
			}
			break;
		default:
			break;
		}
	}

	if ( Input::IsKeyPressed(SE_KEY_LEFT_CONTROL) )
	{
		switch ( event.GetKey() )
		{
		case KeyCode::D:
			if ( m_SelectedEntity )
			{
				m_EditorScene->DuplicateEntity(m_SelectedEntity);
			}
			break;
		case KeyCode::G:
			// Toggle grid
			SceneRenderer::GetOptions().ShowGrid = !SceneRenderer::GetOptions().ShowGrid;
			break;
		case KeyCode::O:
			OpenScenePrompt();
			break;
		case KeyCode::S:
			SaveActiveScene();
			break;
		default:
			break;
		}

		if ( Input::IsKeyPressed(SE_KEY_LEFT_SHIFT) )
		{
			switch ( event.GetKey() )
			{
			case KeyCode::S:
				SaveSceneAsPrompt();
				break;
			default:
				break;
			}
		}
	}

	return false;
}

bool EditorLayer::OnMouseButtonPressed(const MousePressEvent &event)
{
	if ( m_MainViewport.IsFocused() &&
		event.GetButton() == SE_BUTTON_LEFT &&
		!Input::IsKeyPressed(SE_KEY_LEFT_ALT) &&
		!ImGuizmo::IsOver() &&
		m_SceneState != SceneState::Play )
	{
		const auto MousePosition = m_MainViewport.GetMousePosition();
		if ( MousePosition.x > -1.0f && MousePosition.x < 1.0f && MousePosition.y > -1.0f && MousePosition.y < 1.0f )
		{
			auto [origin, direction] = CastRay(MousePosition.x, MousePosition.y);

			if ( m_SelectedEntity )
			{
				OnUnselected(m_SelectedEntity);
			}

			struct Selection
			{
				Entity Entity;
				Submesh *Submesh = nullptr;
				float Distance = 0.0f;
			};

			static std::vector<Selection> selections;
			auto meshEntities = m_EditorScene->GetAllEntitiesWith<MeshComponent>();
			for ( auto e : meshEntities )
			{
				Entity entity = { e, m_EditorScene.Raw() };
				auto mesh = entity.GetComponent<MeshComponent>().Mesh;
				if ( !mesh )
					continue;

				auto &submeshes = mesh->GetSubmeshes();
				for ( Uint32 i = 0; i < submeshes.size(); i++ )
				{
					auto &submesh = submeshes[i];
					Ray ray = {
						glm::inverse(entity.Transform() * submesh.Transform) * glm::vec4(origin, 1.0f),
						glm::inverse(glm::mat3(entity.Transform()) * glm::mat3(submesh.Transform)) * direction
					};

					float t;
					const bool intersects = ray.IntersectsAABB(submesh.BoundingBox, t);
					if ( intersects )
					{
						const auto &triangleCache = mesh->GetTriangleCache(i);
						for ( const auto &triangle : triangleCache )
						{
							if ( ray.IntersectsTriangle(triangle.V0.Position, triangle.V1.Position, triangle.V2.Position, t) )
							{
								selections.push_back({ entity, &submesh, t });
								break;
							}
						}
					}
				}
			}
			std::sort(selections.begin(), selections.end(), [](auto &a, auto &b) { return a.Distance < b.Distance; });
			if ( !selections.empty() )
			{
				OnSelected(selections.front().Entity);
			}
			selections.clear();

		}
	}
	return false;
}

bool EditorLayer::OnWindowDropFiles(const WindowDropFilesEvent &event)
{
	const auto &paths = event.GetPaths();

	// If user dropped a scene in window
	if ( m_SceneState == SceneState::Edit && paths.size() == 1 )
	{
		const auto &scenePath = paths.front();
		if ( scenePath.extension() == ".ssc" )
		{
			if ( scenePath != m_SceneFilePath )
			{
				SaveActiveScene();
				LoadNewScene(scenePath.string());
			}
			else
			{
				SE_INFO("Tried to load a scene that was already active");
			}
		}
	}
	return false;
}

std::pair<glm::vec3, glm::vec3> EditorLayer::CastRay(float mx, float my) const
{
	const glm::vec4 mouseClipPos = { mx, my, -1.0f, 1.0f };

	const auto inverseProj = glm::inverse(m_EditorCamera.GetProjectionMatrix());
	const auto inverseView = glm::inverse(glm::mat3(m_EditorCamera.GetViewMatrix()));

	const glm::vec4 ray = inverseProj * mouseClipPos;
	glm::vec3 rayPos = m_EditorCamera.GetPosition();
	glm::vec3 rayDir = inverseView * glm::vec3(ray);

	return { rayPos, rayDir };
}

void EditorLayer::OnSelected(Entity entity)
{
	m_SelectedEntity = entity;
	m_EntityPanel->SetSelected(entity);
	m_EditorScene->SetSelectedEntity(entity);
}

void EditorLayer::OnUnselected(Entity entity)
{
	SceneRenderer::DisableRenderTarget("MiniRenderTarget");
	m_SelectedEntity = {};
	m_EntityPanel->SetSelected({});
	m_EditorScene->SetSelectedEntity({});
}

void EditorLayer::OnEntityDeleted(Entity entity)
{
	if ( m_SelectedEntity == entity )
	{
		m_EditorScene->SetSelectedEntity({});
	}
}

Ray EditorLayer::CastMouseRay() const
{
	const auto MousePosition = m_MainViewport.GetMousePosition();
	if ( MousePosition.x > -1.0f && MousePosition.x < 1.0f && MousePosition.y > -1.0f && MousePosition.y < 1.0f )
	{
		auto [origin, direction] = CastRay(MousePosition.x, MousePosition.y);
		return Ray(origin, direction);
	}
	return Ray::Zero();
}

}
