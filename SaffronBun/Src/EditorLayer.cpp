#include "EditorLayer.h"

#include <filesystem>

#include "Saffron/Core/Misc.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Gui/GuiTerminal.h"
#include "Saffron/Input/Input.h"
#include "Saffron/Renderer/Renderer2D.h"
#include "Saffron/Script/ScriptEngine.h"
#include "Saffron/System/Macros.h"


namespace Se
{

EditorLayer::EditorLayer()
	:
	m_Style(static_cast<int>(Gui::Style::Dark)),
	m_EditorCamera(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f))
{
	Gui::Init();
	GuiTerminal::Init();
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

	m_SceneHierarchyPanel = CreateScope<SceneHierarchyPanel>(m_EditorScene);
	m_SceneHierarchyPanel->SetSelectionChangedCallback([this](Entity entity) { SelectEntity(entity); });
	m_SceneHierarchyPanel->SetEntityDeletedCallback([this](Entity entity) { OnEntityDeleted(entity); });

	LoadNewScene("Assets/Scenes/Levels/Physics2D-Game.ssc");

	ScriptEngine::SetSceneContext(m_EditorScene);
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
	m_SelectionContext.clear();
}

void EditorLayer::OnScenePlay()
{
	m_SelectionContext.clear();

	m_SceneState = SceneState::Play;

	if ( m_ReloadScriptOnPlay )
		ScriptEngine::ReloadAssembly("Assets/Scripts/ExampleApp.dll");

	m_RuntimeScene = Ref<Scene>::Create();
	m_EditorScene->CopyTo(m_RuntimeScene);

	m_RuntimeScene->OnRuntimeStart();
	m_SceneHierarchyPanel->SetContext(m_RuntimeScene);

	m_GizmoType = -1;
}

void EditorLayer::OnSceneStop()
{
	m_SelectionContext.clear();

	m_RuntimeScene->OnRuntimeStop();
	m_SceneState = SceneState::Edit;

	ScriptEngine::SetSceneContext(m_EditorScene);
	m_SceneHierarchyPanel->SetContext(m_EditorScene);

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
			Renderer::BeginRenderPass(SceneRenderer::GetFinalRenderPass(), false);
			const auto viewProj = m_EditorCamera.GetViewProjection();
			Renderer2D::BeginScene(viewProj, false);
			Renderer::DrawAABB(AABB{ glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3{ 100.0f, 100.0f, 100.0f } }, glm::mat4(1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
			Renderer2D::DrawLine(glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3{ 100.0f, 100.0f, 100.0f }, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			Renderer2D::EndScene();
			Renderer::EndRenderPass();
		}

		if ( !m_SelectionContext.empty() && false )
		{
			auto &selection = m_SelectionContext[0];

			if ( selection.Mesh && selection.Entity.HasComponent<MeshComponent>() )
			{
				Renderer::BeginRenderPass(SceneRenderer::GetFinalRenderPass(), false);
				auto viewProj = m_EditorCamera.GetViewProjection();
				Renderer2D::BeginScene(viewProj, false);
				glm::vec4 color = (m_SelectionMode == SelectionMode::Entity) ? glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f } : glm::vec4{ 0.2f, 0.9f, 0.2f, 1.0f };
				Renderer::DrawAABB(selection.Mesh->BoundingBox, selection.Entity.GetComponent<TransformComponent>().Transform * selection.Mesh->Transform, color);
				Renderer2D::EndScene();
				Renderer::EndRenderPass();
			}
		}

		if ( !m_SelectionContext.empty() )
		{
			auto &selection = m_SelectionContext[0];

			if ( selection.Entity.HasComponent<BoxCollider2DComponent>() )
			{
				const auto &size = selection.Entity.GetComponent<BoxCollider2DComponent>().Size;
				auto [translation, rotationQuat, scale] = Misc::GetTransformDecomposition(selection.Entity.GetComponent<TransformComponent>().Transform);
				const glm::vec3 rotation = glm::eulerAngles(rotationQuat);

				Renderer::BeginRenderPass(SceneRenderer::GetFinalRenderPass(), false);
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
		if ( EditorViewport::Focused )
			m_EditorCamera.OnUpdate(ts);

		m_RuntimeScene->OnRenderRuntime(ts);
		break;
	}
	}
}

void EditorLayer::ShowBoundingBoxes(bool show, bool onTop)
{
	SceneRenderer::GetOptions().ShowBoundingBoxes = show && !onTop;
	m_DrawOnTopBoundingBoxes = show && onTop;
}

void EditorLayer::SelectEntity(Entity entity)
{
	SelectedSubmesh selection;
	if ( entity.HasComponent<MeshComponent>() )
	{
		selection.Mesh = &entity.GetComponent<MeshComponent>().Mesh->GetSubmeshes()[0];
	}
	selection.Entity = entity;
	m_SelectionContext.clear();
	m_SelectionContext.push_back(selection);

	m_EditorScene->SetSelectedEntity(entity);
}

void EditorLayer::NewScenePrompt()
{
	auto &app = Application::Get();
	const std::filesystem::path filepath = app.SaveFile("Saffron Scene (*.ssc)\0*.ssc\0");

	if ( !filepath.empty() )
	{
		std::string sceneName = filepath.stem().string();
		Ref<Scene> newScene = Ref<Scene>::Create(sceneName);

		// Default construct environment and light
		// TODO: Prompt user with templates instead?
		newScene->SetEnvironment(Environment::Load("Assets/Env/birchwood_4k.hdr"));
		const Light light = { {-0.5f, -0.5f, 1.0f}, {1.0f, 1.0f, 1.0f}, 1.0f };
		newScene->SetLight(light);

		m_EditorScene = newScene;
		UpdateWindowTitle(sceneName);
		m_SceneHierarchyPanel->SetContext(m_EditorScene);
		ScriptEngine::SetSceneContext(m_EditorScene);

		m_SceneFilePath = filepath;

		SaveActiveScene();
		OnSceneChange();
	}
}

void EditorLayer::OpenScenePrompt()
{
	auto &app = Application::Get();
	const std::filesystem::path filepath = app.OpenFile("Saffron Scene (*.ssc)\0*.ssc\0");
	if ( !filepath.empty() )
	{
		LoadNewScene(filepath.string());
	}
}

void EditorLayer::SaveSceneAsPrompt()
{
	auto &app = Application::Get();
	const std::filesystem::path filepath = app.SaveFile("Saffron Scene (*.ssc)\0*.ssc\0");
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
		const Ref<Scene> newScene = Ref<Scene>::Create();
		SceneSerializer serializer(newScene);
		if ( !serializer.Deserialize(filepath) )
		{
			SE_WARN("Failed to load scene! Filepath: {0}", filepath);
			return;
		}

		m_EditorScene = newScene;

		UpdateWindowTitle(m_EditorScene->GetName());
		m_SceneHierarchyPanel->SetContext(m_EditorScene);
		ScriptEngine::SetSceneContext(m_EditorScene);

		m_EditorScene->SetSelectedEntity({});
		m_SelectionContext.clear();

		m_SceneFilePath = filepath;

		SaveActiveScene();
		OnSceneChange();
	}
	else
	{
		SE_INFO("Tried to load a scene that was already active");
	}
}

void EditorLayer::OnImGuiRender()
{
	static bool p_open = true;

	static bool opt_fullscreen_persistent = true;
	static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None;
	bool opt_fullscreen = opt_fullscreen_persistent;

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

	ImGui::PopStyleVar();

	if ( opt_fullscreen )
		ImGui::PopStyleVar(2);

	// Dockspace
	ImGuiIO &io = ImGui::GetIO();
	if ( io.ConfigFlags & ImGuiConfigFlags_DockingEnable )
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
	}

	// Editor Panel ------------------------------------------------------------------------------
	ImGui::Begin("Model");
	ImGui::Begin("Environment");

	if ( ImGui::Button("Load Environment Map") )
	{
		std::filesystem::path filepath = Application::Get().OpenFile("*.hdr");
		if ( !filepath.empty() )
			m_EditorScene->SetEnvironment(Environment::Load(filepath.string()));
	}

	ImGui::SliderFloat("Skybox LOD", &m_EditorScene->GetSkyboxLod(), 0.0f, 11.0f);

	ImGui::Columns(2);
	ImGui::AlignTextToFramePadding();

	auto &light = m_EditorScene->GetLight();
	Gui::Property("Light Direction", light.Direction, Gui::PropertyFlag::Slider);
	Gui::Property("Light Radiance", light.Radiance, Gui::PropertyFlag::Color);
	Gui::Property("Light Multiplier", light.Multiplier, 0.0f, 5.0f, Gui::PropertyFlag::Slider);

	Gui::Property("Exposure", m_EditorCamera.GetExposure(), 0.0f, 5.0f, Gui::PropertyFlag::Slider);

	Gui::Property("Radiance Prefiltering", m_RadiancePrefilter);
	Gui::Property("Env Map Rotation", m_EnvMapRotation, -360.0f, 360.0f, Gui::PropertyFlag::Slider);

	if ( m_SceneState == SceneState::Edit )
	{
		float physics2DGravity = m_EditorScene->GetPhysics2DGravity();
		if ( Gui::Property("Gravity", physics2DGravity, -10000.0f, 10000.0f, Gui::PropertyFlag::Drag) )
		{
			m_EditorScene->SetPhysics2DGravity(physics2DGravity);
		}
	}
	else if ( m_SceneState == SceneState::Play )
	{
		float physics2DGravity = m_RuntimeScene->GetPhysics2DGravity();
		if ( Gui::Property("Gravity", physics2DGravity, -10000.0f, 10000.0f, Gui::PropertyFlag::Drag) )
		{
			m_RuntimeScene->SetPhysics2DGravity(physics2DGravity);
		}
	}

	if ( Gui::Property("Show Bounding Boxes", m_UIShowBoundingBoxes) )
		ShowBoundingBoxes(m_UIShowBoundingBoxes, m_UIShowBoundingBoxesOnTop);
	if ( m_UIShowBoundingBoxes && Gui::Property("On Top", m_UIShowBoundingBoxesOnTop) )
		ShowBoundingBoxes(m_UIShowBoundingBoxes, m_UIShowBoundingBoxesOnTop);

	char *label = m_SelectionMode == SelectionMode::Entity ? "Entity" : "Mesh";
	if ( ImGui::Button(label) )
	{
		m_SelectionMode = m_SelectionMode == SelectionMode::Entity ? SelectionMode::SubMesh : SelectionMode::Entity;
	}

	ImGui::Columns(1);

	ImGui::End();

	ImGui::Separator();
	{
		ImGui::Text("Mesh");
		/*auto meshComponent = m_MeshEntity.GetComponent<MeshComponent>();
		std::string fullpath = meshComponent.Mesh ? meshComponent.Mesh->GetFilePath() : "None";
		size_t found = fullpath.find_last_of("/\\");
		std::string path = found != std::string::npos ? fullpath.substr(found + 1) : fullpath;
		ImGui::Text(path.c_str()); ImGui::SameLine();
		if (ImGui::Button("...##Mesh"))
		{
			std::string filename = Application::Get().OpenFile("");
			if (filename != "")
			{
				auto newMesh = Ref<Mesh>::Create(filename);
				// m_MeshMaterial.reset(new MaterialInstance(newMesh->GetMaterial()));
				// m_MeshEntity->SetMaterial(m_MeshMaterial);
				meshComponent.Mesh = newMesh;
			}
		}*/
	}
	ImGui::Separator();

	if ( ImGui::TreeNode("Shaders") )
	{
		auto &shaders = Shader::m_sAllShaders;
		for ( auto &shader : shaders )
		{
			if ( ImGui::TreeNode(shader->GetName().c_str()) )
			{
				std::string buttonName = "Reload##" + shader->GetName();
				if ( ImGui::Button(buttonName.c_str()) )
					shader->Reload();
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}

	ImGui::End();

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
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("Viewport");

	EditorViewport::Hovered = ImGui::IsWindowHovered();
	EditorViewport::Focused = ImGui::IsWindowFocused();
	const auto &tl = EditorViewport::TopLeft;
	const auto &br = EditorViewport::BottomRight;
	if ( EditorViewport::Focused )
	{
		ImGui::GetForegroundDrawList()->AddRect(ImVec2(tl.x, tl.y), ImVec2(br.x, br.y), IM_COL32(255, 140, 0, 180));
	}
	else
	{
		ImGui::GetForegroundDrawList()->AddRect(ImVec2(tl.x, tl.y), ImVec2(br.x, br.y), IM_COL32(255, 140, 0, 80));
	}

	auto viewportOffset = ImGui::GetCursorPos(); // includes tab bar
	auto viewportSize = ImGui::GetContentRegionAvail();
	SceneRenderer::SetViewportSize(static_cast<uint32_t>(viewportSize.x), static_cast<uint32_t>(viewportSize.y));
	m_EditorScene->SetViewportSize(static_cast<uint32_t>(viewportSize.x), static_cast<uint32_t>(viewportSize.y));
	if ( m_RuntimeScene )
		m_RuntimeScene->SetViewportSize(static_cast<uint32_t>(viewportSize.x), static_cast<uint32_t>(viewportSize.y));
	m_EditorCamera.SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), viewportSize.x, viewportSize.y, 0.1f, 10000.0f));
	m_EditorCamera.SetViewportSize(static_cast<uint32_t>(viewportSize.x), static_cast<uint32_t>(viewportSize.y));
	ImGui::Image(reinterpret_cast<void *>(SceneRenderer::GetFinalColorBufferRendererID()), viewportSize, { 0, 1 }, { 1, 0 });

	auto windowSize = ImGui::GetWindowSize();
	ImVec2 minBound = ImGui::GetWindowPos();
	minBound.x += viewportOffset.x;
	minBound.y += viewportOffset.y;

	ImVec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };
	EditorViewport::TopLeft = { minBound.x, minBound.y };
	EditorViewport::BottomRight = { maxBound.x, maxBound.y };
	m_AllowViewportCameraEvents = ImGui::IsMouseHoveringRect(minBound, maxBound);

	// Gizmos
	if ( m_GizmoType != -1 && !m_SelectionContext.empty() )
	{
		auto &selection = m_SelectionContext[0];

		auto rw = static_cast<float>(ImGui::GetWindowWidth());
		auto rh = static_cast<float>(ImGui::GetWindowHeight());
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, rw, rh);

		bool snap = Input::IsKeyPressed(SE_KEY_LEFT_CONTROL);

		auto &entityTransform = selection.Entity.Transform();
		float snapValue = GetSnapValue();
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
			glm::mat4 transformBase = entityTransform * selection.Mesh->Transform;
			ImGuizmo::Manipulate(glm::value_ptr(m_EditorCamera.GetViewMatrix()),
								 glm::value_ptr(m_EditorCamera.GetProjectionMatrix()),
								 static_cast<ImGuizmo::OPERATION>(m_GizmoType),
								 ImGuizmo::LOCAL,
								 glm::value_ptr(transformBase),
								 nullptr,
								 snap ? snapValues : nullptr);

			selection.Mesh->Transform = glm::inverse(entityTransform) * transformBase;
		}
	}

	ImGui::End();
	ImGui::PopStyleVar();

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

	m_SceneHierarchyPanel->OnImGuiRender();

	ImGui::Begin("Materials");

	if ( !m_SelectionContext.empty() )
	{
		Entity selectedEntity = m_SelectionContext.front().Entity;
		if ( selectedEntity.HasComponent<MeshComponent>() )
		{
			Ref<Mesh> mesh = selectedEntity.GetComponent<MeshComponent>().Mesh;
			if ( mesh )
			{
				auto materials = mesh->GetMaterials();
				static uint32_t selectedMaterialIndex = 0;
				for ( uint32_t i = 0; i < materials.size(); i++ )
				{
					auto &materialInstance = materials[i];

					ImGuiTreeNodeFlags node_flags = (selectedMaterialIndex == i ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_Leaf;
					bool opened = ImGui::TreeNodeEx(static_cast<void *>(&materialInstance), node_flags, materialInstance->GetName().c_str());
					if ( ImGui::IsItemClicked() )
					{
						selectedMaterialIndex = i;
					}
					if ( opened )
						ImGui::TreePop();

				}

				ImGui::Separator();

				// Selected material
				if ( selectedMaterialIndex < materials.size() )
				{
					auto &materialInstance = materials[selectedMaterialIndex];
					ImGui::Text("Shader: %s", materialInstance->GetShader()->GetName().c_str());
					// Textures ------------------------------------------------------------------------------
					{
						// Albedo
						if ( ImGui::CollapsingHeader("Albedo", nullptr, ImGuiTreeNodeFlags_DefaultOpen) )
						{
							ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));

							auto &albedoColor = materialInstance->Get<glm::vec3>("u_AlbedoColor");
							bool useAlbedoMap = materialInstance->Get<float>("u_AlbedoTexToggle");
							Ref<Texture2D> albedoMap = materialInstance->TryGetResource<Texture2D>("u_AlbedoTexture");
							ImGui::Image(albedoMap ? reinterpret_cast<void *>(albedoMap->GetRendererID()) : reinterpret_cast<void *>(m_TexStore["Checkerboard"]->GetRendererID()), ImVec2(64, 64));
							ImGui::PopStyleVar();
							if ( ImGui::IsItemHovered() )
							{
								if ( albedoMap )
								{
									ImGui::BeginTooltip();
									ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
									ImGui::TextUnformatted(albedoMap->GetPath().c_str());
									ImGui::PopTextWrapPos();
									ImGui::Image(reinterpret_cast<void *>(albedoMap->GetRendererID()), ImVec2(384, 384));
									ImGui::EndTooltip();
								}
								if ( ImGui::IsItemClicked() )
								{
									std::filesystem::path filepath = Application::Get().OpenFile("");
									if ( !filepath.empty() )
									{
										albedoMap = Texture2D::Create(filepath.string(), true/*m_AlbedoInput.sRGB*/);
										materialInstance->Set("u_AlbedoTexture", albedoMap);
									}
								}
							}
							ImGui::SameLine();
							ImGui::BeginGroup();
							if ( ImGui::Checkbox("Use##AlbedoMap", &useAlbedoMap) )
								materialInstance->Set<float>("u_AlbedoTexToggle", useAlbedoMap ? 1.0f : 0.0f);

							/*if (ImGui::Checkbox("sRGB##AlbedoMap", &m_AlbedoInput.sRGB))
							{
								if (m_AlbedoInput.TextureMap)
									m_AlbedoInput.TextureMap = Texture2D::Create(m_AlbedoInput.TextureMap->GetPath(), m_AlbedoInput.sRGB);
							}*/
							ImGui::EndGroup();
							ImGui::SameLine();
							ImGui::ColorEdit3("Color##Albedo", glm::value_ptr(albedoColor), ImGuiColorEditFlags_NoInputs);
						}
					}
					{
						// Normals
						if ( ImGui::CollapsingHeader("Normals", nullptr, ImGuiTreeNodeFlags_DefaultOpen) )
						{
							ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
							bool useNormalMap = materialInstance->Get<float>("u_NormalTexToggle");
							Ref<Texture2D> normalMap = materialInstance->TryGetResource<Texture2D>("u_NormalTexture");
							ImGui::Image(normalMap ? reinterpret_cast<void *>(normalMap->GetRendererID()) : reinterpret_cast<void *>(m_TexStore["Checkerboard"]->GetRendererID()), ImVec2(64, 64));
							ImGui::PopStyleVar();
							if ( ImGui::IsItemHovered() )
							{
								if ( normalMap )
								{
									ImGui::BeginTooltip();
									ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
									ImGui::TextUnformatted(normalMap->GetPath().c_str());
									ImGui::PopTextWrapPos();
									ImGui::Image(reinterpret_cast<void *>(normalMap->GetRendererID()), ImVec2(384, 384));
									ImGui::EndTooltip();
								}
								if ( ImGui::IsItemClicked() )
								{
									std::filesystem::path filepath = Application::Get().OpenFile("");
									if ( !filepath.empty() )
									{
										normalMap = Texture2D::Create(filepath.string());
										materialInstance->Set("u_NormalTexture", normalMap);
									}
								}
							}
							ImGui::SameLine();
							if ( ImGui::Checkbox("Use##NormalMap", &useNormalMap) )
								materialInstance->Set<float>("u_NormalTexToggle", useNormalMap ? 1.0f : 0.0f);
						}
					}
					{
						// Metalness
						if ( ImGui::CollapsingHeader("Metalness", nullptr, ImGuiTreeNodeFlags_DefaultOpen) )
						{
							ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
							auto &metalnessValue = materialInstance->Get<float>("u_Metalness");
							bool useMetalnessMap = materialInstance->Get<float>("u_MetalnessTexToggle");
							Ref<Texture2D> metalnessMap = materialInstance->TryGetResource<Texture2D>("u_MetalnessTexture");
							ImGui::Image(metalnessMap ? reinterpret_cast<void *>(metalnessMap->GetRendererID()) : reinterpret_cast<void *>(m_TexStore["Checkerboard"]->GetRendererID()), ImVec2(64, 64));
							ImGui::PopStyleVar();
							if ( ImGui::IsItemHovered() )
							{
								if ( metalnessMap )
								{
									ImGui::BeginTooltip();
									ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
									ImGui::TextUnformatted(metalnessMap->GetPath().c_str());
									ImGui::PopTextWrapPos();
									ImGui::Image(reinterpret_cast<void *>(metalnessMap->GetRendererID()), ImVec2(384, 384));
									ImGui::EndTooltip();
								}
								if ( ImGui::IsItemClicked() )
								{
									std::filesystem::path filepath = Application::Get().OpenFile("");
									if ( !filepath.empty() )
									{
										metalnessMap = Texture2D::Create(filepath.string());
										materialInstance->Set("u_MetalnessTexture", metalnessMap);
									}
								}
							}
							ImGui::SameLine();
							if ( ImGui::Checkbox("Use##MetalnessMap", &useMetalnessMap) )
								materialInstance->Set<float>("u_MetalnessTexToggle", useMetalnessMap ? 1.0f : 0.0f);
							ImGui::SameLine();
							ImGui::SliderFloat("Value##MetalnessInput", &metalnessValue, 0.0f, 1.0f);
						}
					}
					{
						// Roughness
						if ( ImGui::CollapsingHeader("Roughness", nullptr, ImGuiTreeNodeFlags_DefaultOpen) )
						{
							ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
							auto &roughnessValue = materialInstance->Get<float>("u_Roughness");
							bool useRoughnessMap = materialInstance->Get<float>("u_RoughnessTexToggle");
							Ref<Texture2D> roughnessMap = materialInstance->TryGetResource<Texture2D>("u_RoughnessTexture");
							ImGui::Image(roughnessMap ? reinterpret_cast<void *>(roughnessMap->GetRendererID()) : reinterpret_cast<void *>(m_TexStore["Checkerboard"]->GetRendererID()), ImVec2(64, 64));
							ImGui::PopStyleVar();
							if ( ImGui::IsItemHovered() )
							{
								if ( roughnessMap )
								{
									ImGui::BeginTooltip();
									ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
									ImGui::TextUnformatted(roughnessMap->GetPath().c_str());
									ImGui::PopTextWrapPos();
									ImGui::Image(reinterpret_cast<void *>(roughnessMap->GetRendererID()), ImVec2(384, 384));
									ImGui::EndTooltip();
								}
								if ( ImGui::IsItemClicked() )
								{
									std::filesystem::path filepath = Application::Get().OpenFile("");
									if ( !filepath.empty() )
									{
										roughnessMap = Texture2D::Create(filepath.string());
										materialInstance->Set("u_RoughnessTexture", roughnessMap);
									}
								}
							}
							ImGui::SameLine();
							if ( ImGui::Checkbox("Use##RoughnessMap", &useRoughnessMap) )
								materialInstance->Set<float>("u_RoughnessTexToggle", useRoughnessMap ? 1.0f : 0.0f);
							ImGui::SameLine();
							ImGui::SliderFloat("Value##RoughnessInput", &roughnessValue, 0.0f, 1.0f);
						}
					}
				}
			}
		}
	}

	ImGui::End();

	ScriptEngine::OnImGuiRender();
	GuiTerminal::OnImGuiRender();
	Renderer::OnImGuiRender();

	ImGui::End();
}

void EditorLayer::OnEvent(const Event &event)
{
	if ( m_SceneState == SceneState::Edit )
	{
		if ( EditorViewport::Hovered )
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
	if ( EditorViewport::Focused )
	{
		switch ( event.GetKey() )
		{
		case KeyCode::Delete:
			if ( !m_SelectionContext.empty() )
			{
				const Entity selectedEntity = m_SelectionContext[0].Entity;
				m_EditorScene->DestroyEntity(selectedEntity);
				m_SelectionContext.clear();
				m_EditorScene->SetSelectedEntity({});
				m_SceneHierarchyPanel->SetSelected({});
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
		case KeyCode::B:
			// Toggle bounding boxes 
			m_UIShowBoundingBoxes = !m_UIShowBoundingBoxes;
			ShowBoundingBoxes(m_UIShowBoundingBoxes, m_UIShowBoundingBoxesOnTop);
			break;
		case KeyCode::D:
			if ( !m_SelectionContext.empty() )
			{
				const Entity selectedEntity = m_SelectionContext[0].Entity;
				m_EditorScene->DuplicateEntity(selectedEntity);
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
	if ( EditorViewport::Focused &&
		event.GetButton() == SE_BUTTON_LEFT &&
		!Input::IsKeyPressed(SE_KEY_LEFT_ALT) &&
		!ImGuizmo::IsOver() &&
		m_SceneState != SceneState::Play )
	{
		const auto MousePosition = EditorViewport::GetMousePosition();
		if ( MousePosition.x > -1.0f && MousePosition.x < 1.0f && MousePosition.y > -1.0f && MousePosition.y < 1.0f )
		{
			auto [origin, direction] = CastRay(MousePosition.x, MousePosition.y);

			m_SelectionContext.clear();
			m_EditorScene->SetSelectedEntity({});
			auto meshEntities = m_EditorScene->GetAllEntitiesWith<MeshComponent>();
			for ( auto e : meshEntities )
			{
				Entity entity = { e, m_EditorScene.Raw() };
				auto mesh = entity.GetComponent<MeshComponent>().Mesh;
				if ( !mesh )
					continue;

				auto &submeshes = mesh->GetSubmeshes();
				for ( uint32_t i = 0; i < submeshes.size(); i++ )
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
								SE_WARN("INTERSECTION: {0}, t={1}", submesh.NodeName, t);
								m_SelectionContext.push_back({ entity, &submesh, t });
								break;
							}
						}
					}
				}
			}
			std::sort(m_SelectionContext.begin(), m_SelectionContext.end(), [](auto &a, auto &b) { return a.Distance < b.Distance; });
			if ( !m_SelectionContext.empty() )
				OnSelected(m_SelectionContext[0]);

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

void EditorLayer::OnSelected(const SelectedSubmesh &selectionContext)
{
	m_SceneHierarchyPanel->SetSelected(selectionContext.Entity);
	m_EditorScene->SetSelectedEntity(selectionContext.Entity);
}

void EditorLayer::OnEntityDeleted(Entity e)
{
	if ( m_SelectionContext[0].Entity == e )
	{
		m_SelectionContext.clear();
		m_EditorScene->SetSelectedEntity({});
	}
}

Ray EditorLayer::CastMouseRay() const
{
	const auto MousePosition = EditorViewport::GetMousePosition();
	if ( MousePosition.x > -1.0f && MousePosition.x < 1.0f && MousePosition.y > -1.0f && MousePosition.y < 1.0f )
	{
		auto [origin, direction] = CastRay(MousePosition.x, MousePosition.y);
		return Ray(origin, direction);
	}
	return Ray::Zero();
}

}
