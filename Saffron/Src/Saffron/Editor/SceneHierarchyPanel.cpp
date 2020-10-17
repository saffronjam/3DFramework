#include "SaffronPCH.h"

#include <assimp/scene.h>

#include "Saffron/Core/Application.h"
#include "Saffron/Core/Math/SaffronMath.h"
#include "Saffron/Editor/SceneHierarchyPanel.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Script/ScriptEngine.h"
#include "Saffron/Script/ScriptManager.h"


namespace Se
{
glm::mat4 Mat4FromAssimpMat4(const aiMatrix4x4 &matrix);


///////////////////////////////////////////////////////////////////////////
/// Statics
///////////////////////////////////////////////////////////////////////////

static int s_UIContextID = 0;
static Uint32 s_Counter = 0;
static char s_IDBuffer[16];


///////////////////////////////////////////////////////////////////////////
/// Helper structs
///////////////////////////////////////////////////////////////////////////

static void PushID()
{
	ImGui::PushID(s_UIContextID++);
	s_Counter = 0;
}

static void PopID()
{
	ImGui::PopID();
	s_UIContextID--;
}

static void BeginPropertyGrid(float width = -1.0)
{
	PushID();
	ImGui::Columns(2);
}

static void EndPropertyGrid()
{
	ImGui::Columns(1);
	PopID();
}

static void InfoModal(const char *title, const char *text, bool open)
{
	if ( open )
	{
		PushID();

		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		ImGui::OpenPopup(title);

		if ( ImGui::BeginPopupModal(title) )
		{
			ImGui::Text(text);
			if ( ImGui::Button("Dismiss") )
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		PopID();
	}
}


///////////////////////////////////////////////////////////////////////////
/// Helper functions
///////////////////////////////////////////////////////////////////////////

static std::tuple<glm::vec3, glm::quat, glm::vec3> GetTransformDecomposition(const glm::mat4 &transform)
{
	glm::vec3 scale, translation, skew;
	glm::vec4 perspective;
	glm::quat orientation;
	glm::decompose(transform, scale, orientation, translation, skew, perspective);

	return { translation, orientation, scale };
}


template<typename T, typename UIFunction>
static void DrawComponent(const std::string &name, Entity entity, UIFunction uiFunction)
{
	if ( entity.HasComponent<T>() )
	{
		bool removeComponent = false;

		auto &component = entity.GetComponent<T>();
		const bool open = ImGui::TreeNodeEx(reinterpret_cast<void *>(static_cast<Uint32>(entity) | typeid(T).hash_code()), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap, name.c_str());
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
		if ( ImGui::Button("+") )
		{
			ImGui::OpenPopup("ComponentSettings");
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		if ( ImGui::BeginPopup("ComponentSettings") )
		{
			if ( ImGui::MenuItem("Remove component") )
				removeComponent = true;

			ImGui::EndPopup();
		}

		if ( open )
		{
			uiFunction(component);
			ImGui::NextColumn();
			ImGui::Columns(1);
			ImGui::TreePop();
		}
		ImGui::Separator();

		if ( removeComponent )
			entity.RemoveComponent<T>();
	}
}


///////////////////////////////////////////////////////////////////////////
/// Helper functions - PROPERTIES
///////////////////////////////////////////////////////////////////////////

static void Property(const char *label, const char *value)
{
	ImGui::Text(label);
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	s_IDBuffer[0] = '#';
	s_IDBuffer[1] = '#';
	memset(s_IDBuffer + 2, 0, 14);
	_itoa(s_Counter++, s_IDBuffer + 2, 16);
	ImGui::InputText(s_IDBuffer, const_cast<char *>(value), 256, ImGuiInputTextFlags_ReadOnly);

	ImGui::PopItemWidth();
	ImGui::NextColumn();
}

static bool Property(const char *label, std::string &value, bool error = false)
{
	bool modified = false;

	ImGui::Text(label);
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	char buffer[256];
	strcpy(buffer, value.c_str());

	s_IDBuffer[0] = '#';
	s_IDBuffer[1] = '#';
	memset(s_IDBuffer + 2, 0, 14);
	_itoa(s_Counter++, s_IDBuffer + 2, 16);

	if ( error )
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
	if ( ImGui::InputText(s_IDBuffer, buffer, 256) )
	{
		value = buffer;
		modified = true;
	}
	if ( error )
		ImGui::PopStyleColor();
	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return modified;
}
static bool Property(const char *label, bool &value)
{
	bool modified = false;

	ImGui::Text(label);
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	s_IDBuffer[0] = '#';
	s_IDBuffer[1] = '#';
	memset(s_IDBuffer + 2, 0, 14);
	_itoa(s_Counter++, s_IDBuffer + 2, 16);
	if ( ImGui::Checkbox(s_IDBuffer, &value) )
		modified = true;

	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return modified;
}

static bool Property(const char *label, int &value)
{
	bool modified = false;

	ImGui::Text(label);
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	s_IDBuffer[0] = '#';
	s_IDBuffer[1] = '#';
	memset(s_IDBuffer + 2, 0, 14);
	_itoa(s_Counter++, s_IDBuffer + 2, 16);
	if ( ImGui::DragInt(s_IDBuffer, &value) )
		modified = true;

	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return modified;
}

static bool Property(const char *label, float &value, float delta = 0.1f)
{
	bool modified = false;

	ImGui::Text(label);
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	s_IDBuffer[0] = '#';
	s_IDBuffer[1] = '#';
	memset(s_IDBuffer + 2, 0, 14);
	_itoa(s_Counter++, s_IDBuffer + 2, 16);
	if ( ImGui::DragFloat(s_IDBuffer, &value, delta) )
		modified = true;

	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return modified;
}

static bool Property(const char *label, glm::vec2 &value, float delta = 0.1f)
{
	bool modified = false;

	ImGui::Text(label);
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	s_IDBuffer[0] = '#';
	s_IDBuffer[1] = '#';
	memset(s_IDBuffer + 2, 0, 14);
	_itoa(s_Counter++, s_IDBuffer + 2, 16);
	if ( ImGui::DragFloat2(s_IDBuffer, glm::value_ptr(value), delta) )
		modified = true;

	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return modified;
}

static bool Property(const char *label, glm::vec3 &value, float delta = 0.1f)
{
	bool modified = false;

	ImGui::Text(label);
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	s_IDBuffer[0] = '#';
	s_IDBuffer[1] = '#';
	memset(s_IDBuffer + 2, 0, 14);
	_itoa(s_Counter++, s_IDBuffer + 2, 16);
	if ( ImGui::DragFloat3(s_IDBuffer, glm::value_ptr(value), delta) )
		modified = true;

	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return modified;
}

static bool Property(const char *label, glm::vec4 &value, float delta = 0.1f)
{
	bool modified = false;

	ImGui::Text(label);
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	s_IDBuffer[0] = '#';
	s_IDBuffer[1] = '#';
	memset(s_IDBuffer + 2, 0, 14);
	_itoa(s_Counter++, s_IDBuffer + 2, 16);
	if ( ImGui::DragFloat4(s_IDBuffer, glm::value_ptr(value), delta) )
		modified = true;

	ImGui::PopItemWidth();
	ImGui::NextColumn();

	return modified;
}




///////////////////////////////////////////////////////////////////////////
/// Scene Hierarchy Panel
///////////////////////////////////////////////////////////////////////////

SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene> &context)
	: m_Context(context)
{
}

void SceneHierarchyPanel::SetContext(const Ref<Scene> &context)
{
	m_Context = context;
	if ( m_SelectionContext )
	{
		// Try and find same entity in new scene
		const auto &entityMap = m_Context->GetEntityMap();
		const UUID selectedEntityUUID = m_SelectionContext.GetUUID();
		if ( entityMap.find(selectedEntityUUID) != entityMap.end() )
			m_SelectionContext = entityMap.at(selectedEntityUUID);
	}

}

void SceneHierarchyPanel::SetSelected(Entity entity)
{
	m_SelectionContext = entity;
}

void SceneHierarchyPanel::OnImGuiRender()
{
	ImGui::Begin("Scene Hierarchy");
	if ( m_Context )
	{
		m_Context->m_Registry.each([&](auto entity)
								   {
									   Entity e(entity, m_Context.Raw());
									   if ( e.HasComponent<IDComponent>() )
										   DrawEntityNode(e);
								   });

		bool createNewEntity = false;
		bool failedCreateEntity = false;
		if ( ImGui::BeginPopupContextWindow("Create Entity Context", 1, false) )
		{
			if ( ImGui::MenuItem("Create Entity") )
			{
				createNewEntity = true;
			}
			ImGui::EndPopup();
		}
		if ( createNewEntity )
		{
			ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			ImGui::OpenPopup("Create new Entity");
		}

		ImGui::SetNextWindowContentSize(ImVec2(400, 0.0f));
		if ( ImGui::BeginPopupModal("Create new Entity", nullptr, ImGuiWindowFlags_AlwaysAutoResize) )
		{
			static std::string entityName;
			static bool meshComponent = false;
			static bool scriptComponent = false;
			static int scriptChosen = 0;
			static bool cameraComponent = false;
			static bool spriteRendererComponent = false;
			static bool rigidBody2DComponent = false;
			static bool boxCollider2DComponent = false;
			static bool circleCollider2DComponent = false;

			bool badEntityName = false;

			BeginPropertyGrid();
			Property("Name", entityName);
			Property("Mesh", meshComponent);
			Property("Script", scriptComponent);
			if ( scriptComponent )
			{
				ImGui::NextColumn();
				std::ostringstream oss;
				for ( const auto &scriptName : ScriptManager::GetScriptNames() ) { oss << scriptName.Class << '\0'; }
				oss << '\0';
				ImGui::Combo("##EntityCreateScriptComboOption", &scriptChosen, oss.str().c_str());
				ImGui::NextColumn();
			}
			Property("Camera", cameraComponent);
			Property("Sprite", spriteRendererComponent);
			Property("Rigid Body 2D", rigidBody2DComponent);
			Property("Box Collider 2D", boxCollider2DComponent);
			Property("Circle Collider 2D", circleCollider2DComponent);

			if ( ImGui::Button("Cancel") )
			{
				scriptChosen = 0;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine(0, 5);
			if ( ImGui::Button("Create") )
			{
				if ( entityName.empty() )
				{
					badEntityName = true;
				}
				else
				{
					Entity newEntity = m_Context->CreateEntity(entityName);
					if ( meshComponent )
					{
						const std::string defaultMeshPath = "Assets/meshes/Cube1m.fbx";
						newEntity.AddComponent<MeshComponent>(Ref<Mesh>::Create(defaultMeshPath));
						meshComponent = false;
					}
					if ( scriptComponent )
					{
						newEntity.AddComponent<ScriptComponent>(ScriptManager::GetScriptNames().at(scriptChosen).Full);
						scriptComponent = false;
					}
					if ( cameraComponent )
					{
						newEntity.AddComponent<CameraComponent>();
						cameraComponent = false;
					}
					if ( spriteRendererComponent )
					{
						newEntity.AddComponent<SpriteRendererComponent>();
						spriteRendererComponent = false;
					}
					if ( rigidBody2DComponent )
					{
						newEntity.AddComponent<RigidBody2DComponent>();
						rigidBody2DComponent = false;
					}
					if ( boxCollider2DComponent )
					{
						newEntity.AddComponent<BoxCollider2DComponent>();
						boxCollider2DComponent = false;
					}
					if ( circleCollider2DComponent )
					{
						newEntity.AddComponent<CircleCollider2DComponent>();
						circleCollider2DComponent = false;
					}
					entityName.clear();
					ImGui::CloseCurrentPopup();
				}
			}


			EndPropertyGrid();

			ImGui::EndPopup();
		}

		ImGui::End();

		ImGui::Begin("Properties");

		if ( m_SelectionContext )
		{
			DrawComponents(m_SelectionContext);

			if ( ImGui::Button("Add Component") )
				ImGui::OpenPopup("AddComponentPanel");

			if ( ImGui::BeginPopup("AddComponentPanel") )
			{
				if ( !m_SelectionContext.HasComponent<CameraComponent>() )
				{
					if ( ImGui::Button("Camera") )
					{
						m_SelectionContext.AddComponent<CameraComponent>();
						ImGui::CloseCurrentPopup();
					}
				}
				if ( !m_SelectionContext.HasComponent<MeshComponent>() )
				{
					if ( ImGui::Button("Mesh") )
					{
						const std::string defaultMeshPath = "Assets/meshes/Cube1m.fbx";
						m_SelectionContext.AddComponent<MeshComponent>(Ref<Mesh>::Create(defaultMeshPath));
						ImGui::CloseCurrentPopup();
					}
				}
				if ( !m_SelectionContext.HasComponent<ScriptComponent>() )
				{
					if ( ImGui::Button("Script") )
					{
						m_SelectionContext.AddComponent<ScriptComponent>();
						ImGui::CloseCurrentPopup();
					}
				}
				if ( !m_SelectionContext.HasComponent<SpriteRendererComponent>() )
				{
					if ( ImGui::Button("Sprite Renderer") )
					{
						m_SelectionContext.AddComponent<SpriteRendererComponent>();
						ImGui::CloseCurrentPopup();
					}
				}
				if ( !m_SelectionContext.HasComponent<RigidBody2DComponent>() )
				{
					if ( ImGui::Button("Rigidbody 2D") )
					{
						m_SelectionContext.AddComponent<RigidBody2DComponent>();
						ImGui::CloseCurrentPopup();
					}
				}
				if ( !m_SelectionContext.HasComponent<BoxCollider2DComponent>() )
				{
					if ( ImGui::Button("Box Collider 2D") )
					{
						m_SelectionContext.AddComponent<BoxCollider2DComponent>();
						ImGui::CloseCurrentPopup();
					}
				}
				if ( !m_SelectionContext.HasComponent<CircleCollider2DComponent>() )
				{
					if ( ImGui::Button("Circle Collider 2D") )
					{
						m_SelectionContext.AddComponent<CircleCollider2DComponent>();
						ImGui::CloseCurrentPopup();
					}
				}
				ImGui::EndPopup();
			}
		}
	}

	ImGui::End();

	ImGui::Begin("Mesh Debug");
	if ( !m_SelectionContext )
	{
		ImGui::Text("No selected entity");
	}
	else if ( m_SelectionContext.HasComponent<MeshComponent>() )
	{
		Ref<Mesh> mesh = m_SelectionContext.GetComponent<MeshComponent>().Mesh;
		ImGui::TextWrapped("File: %s", mesh->GetFilePath().c_str());

		if ( mesh->m_IsAnimated )
		{
			if ( ImGui::CollapsingHeader("Animation") )
			{
				if ( ImGui::Button(mesh->m_AnimationPlaying ? "Pause" : "Play") )
					mesh->m_AnimationPlaying = !mesh->m_AnimationPlaying;

				ImGui::SliderFloat("##AnimationTime", &mesh->m_AnimationTime, 0.0f, (float)mesh->m_Scene->mAnimations[0]->mDuration);
				ImGui::DragFloat("Time Scale", &mesh->m_TimeMultiplier, 0.05f, 0.0f, 10.0f);
			}
		}
	}
	else
	{
		ImGui::Text("Selected Entity has no mesh");
	}
	ImGui::End();
}

void SceneHierarchyPanel::DrawEntityNode(Entity entity)
{
	std::string name = "Unnamed";
	if ( entity.HasComponent<TagComponent>() )
		name = entity.GetComponent<TagComponent>().Tag;

	const ImGuiTreeNodeFlags node_flags = (entity == m_SelectionContext ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;

	const bool opened = ImGui::TreeNodeEx(reinterpret_cast<void *>(&entity.GetComponent<IDComponent>().ID), node_flags, name.c_str());
	if ( ImGui::IsItemClicked() )
	{
		m_SelectionContext = entity;
		if ( m_SelectionChangedCallback )
			m_SelectionChangedCallback(m_SelectionContext);
	}

	bool entityDeleted = false;
	if ( ImGui::BeginPopupContextItem() )
	{
		if ( ImGui::MenuItem("Delete") )
			entityDeleted = true;

		ImGui::EndPopup();
	}
	if ( opened )
	{
		if ( entity.HasComponent<MeshComponent>() )
		{
			const auto mesh = entity.GetComponent<MeshComponent>().Mesh;
			if ( mesh )
				DrawMeshNode(mesh, entity.GetComponent<IDComponent>().ID);
		}
		ImGui::TreePop();
	}

	// Defer deletion until end of node UI
	if ( entityDeleted )
	{
		m_Context->DestroyEntity(entity);
		if ( entity == m_SelectionContext )
			m_SelectionContext = {};

		m_EntityDeletedCallback(entity);
	}
}

void SceneHierarchyPanel::DrawMeshNode(const Ref<Mesh> &mesh, UUID &entityUUID) const
{
	std::ostringstream oss;
	oss << "Mesh##" << entityUUID;

	// Mesh Hierarchy
	if ( ImGui::TreeNode(oss.str().c_str()) )
	{
		auto *rootNode = mesh->m_Scene->mRootNode;
		MeshNodeHierarchy(mesh, rootNode);
		ImGui::TreePop();
	}
}

void SceneHierarchyPanel::MeshNodeHierarchy(const Ref<Mesh> &mesh,
											aiNode *node,
											const glm::mat4 &parentTransform,
											Uint32 level) const
{
	const glm::mat4 localTransform = Mat4FromAssimpMat4(node->mTransformation);
	const glm::mat4 transform = parentTransform * localTransform;

	if ( ImGui::TreeNode(node->mName.C_Str()) )
	{
		{
			auto [translation, rotation, scale] = GetTransformDecomposition(transform);
			ImGui::Text("World Transform");
			ImGui::Text("  Translation: %.2f, %.2f, %.2f", translation.x, translation.y, translation.z);
			ImGui::Text("  Scale: %.2f, %.2f, %.2f", scale.x, scale.y, scale.z);
		}
		{
			auto [translation, rotation, scale] = GetTransformDecomposition(localTransform);
			ImGui::Text("Local Transform");
			ImGui::Text("  Translation: %.2f, %.2f, %.2f", translation.x, translation.y, translation.z);
			ImGui::Text("  Scale: %.2f, %.2f, %.2f", scale.x, scale.y, scale.z);
		}

		for ( Uint32 i = 0; i < node->mNumChildren; i++ )
			MeshNodeHierarchy(mesh, node->mChildren[i], transform, level + 1);

		ImGui::TreePop();
	}
}

void SceneHierarchyPanel::DrawComponents(Entity entity)
{
	ImGui::AlignTextToFramePadding();

	const auto id = entity.GetComponent<IDComponent>().ID;

	if ( entity.HasComponent<TagComponent>() )
	{
		auto &tag = entity.GetComponent<TagComponent>().Tag;
		char buffer[256];
		memset(buffer, 0, 256);
		memcpy(buffer, tag.c_str(), tag.length());
		if ( ImGui::InputText("##Tag", buffer, 256) )
		{
			tag = std::string(buffer);
		}
	}

	// ID
	ImGui::SameLine();
	ImGui::TextDisabled("%llx", static_cast<Uint64>(id));

	ImGui::Separator();

	if ( entity.HasComponent<TransformComponent>() )
	{
		auto &tc = entity.GetComponent<TransformComponent>();
		if ( ImGui::TreeNodeEx(reinterpret_cast<void *>(static_cast<Uint32>(entity) | typeid(TransformComponent).hash_code()), ImGuiTreeNodeFlags_DefaultOpen, "Transform") )
		{
			auto [translation, rotationQuat, scale] = GetTransformDecomposition(tc);
			glm::vec3 rotation = glm::degrees(glm::eulerAngles(rotationQuat));

			ImGui::Columns(2);
			ImGui::Text("Translation");
			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);

			bool updateTransform = false;

			if ( ImGui::DragFloat3("##translation", glm::value_ptr(translation), 0.25f) )
			{
				//tc.Transform[3] = glm::vec4(translation, 1.0f);
				updateTransform = true;
			}

			ImGui::PopItemWidth();
			ImGui::NextColumn();

			ImGui::Text("Rotation");
			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);

			if ( ImGui::DragFloat3("##rotation", glm::value_ptr(rotation), 0.25f) )
			{
				updateTransform = true;
				// tc.Transform[3] = glm::vec4(translation, 1.0f);
			}

			ImGui::PopItemWidth();
			ImGui::NextColumn();

			ImGui::Text("Scale");
			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);

			if ( ImGui::DragFloat3("##scale", glm::value_ptr(scale), 0.25f) )
			{
				updateTransform = true;
			}

			ImGui::PopItemWidth();
			ImGui::NextColumn();

			ImGui::Columns(1);

			if ( updateTransform )
			{
				tc.Transform = glm::translate(glm::mat4(1.0f), translation) *
					glm::toMat4(glm::quat(glm::radians(rotation))) *
					glm::scale(glm::mat4(1.0f), scale);
			}

			ImGui::TreePop();
		}
		ImGui::Separator();
	}

	DrawComponent<MeshComponent>("Mesh", entity, [](MeshComponent &mc)
								 {
									 ImGui::Columns(3);
									 ImGui::SetColumnWidth(0, 100);
									 ImGui::SetColumnWidth(1, 300);
									 ImGui::SetColumnWidth(2, 40);
									 ImGui::Text("File Path");
									 ImGui::NextColumn();
									 ImGui::PushItemWidth(-1);
									 if ( mc.Mesh )
										 ImGui::InputText("##meshfilepath", const_cast<char *>(mc.Mesh->GetFilePath().c_str()), 256, ImGuiInputTextFlags_ReadOnly);
									 else
										 ImGui::InputText("##meshfilepath", static_cast<char *>("Null"), 256, ImGuiInputTextFlags_ReadOnly);
									 ImGui::PopItemWidth();
									 ImGui::NextColumn();
									 if ( ImGui::Button("...##openmesh") )
									 {
										 const std::filesystem::path filepath = Application::Get().OpenFile();
										 if ( !filepath.empty() )
											 mc.Mesh = Ref<Mesh>::Create(filepath.string());
									 }
								 });

	DrawComponent<CameraComponent>("Camera", entity, [](CameraComponent &cc)
								   {
									   // Projection Type
									   const char *projTypeStrings[] = { "Perspective", "Orthographic" };
									   const char *currentProj = projTypeStrings[static_cast<int>(cc.Camera->GetProjectionMode())];
									   if ( ImGui::BeginCombo("Projection", currentProj) )
									   {
										   for ( int type = 0; type < 2; type++ )
										   {
											   const bool is_selected = (currentProj == projTypeStrings[type]);
											   if ( ImGui::Selectable(projTypeStrings[type], is_selected) )
											   {
												   currentProj = projTypeStrings[type];
												   cc.Camera->SetProjectionMode(static_cast<SceneCamera::ProjectionMode>(type));
											   }
											   if ( is_selected )
												   ImGui::SetItemDefaultFocus();
										   }
										   ImGui::EndCombo();
									   }

									   BeginPropertyGrid();
									   // Perspective parameters
									   if ( cc.Camera->GetProjectionMode() == SceneCamera::ProjectionMode::Perspective )
									   {
										   float verticalFOV = cc.Camera->GetPerspectiveVerticalFOV();
										   if ( Property("Vertical FOV", verticalFOV) )
											   cc.Camera->SetPerspectiveVerticalFOV(verticalFOV);

										   float nearClip = cc.Camera->GetPerspectiveNearClip();
										   if ( Property("Near Clip", nearClip) )
											   cc.Camera->SetPerspectiveNearClip(nearClip);
										   ImGui::SameLine();
										   float farClip = cc.Camera->GetPerspectiveFarClip();
										   if ( Property("Far Clip", farClip) )
											   cc.Camera->SetPerspectiveFarClip(farClip);
									   }

									   // Orthographic parameters
									   else if ( cc.Camera->GetProjectionMode() == SceneCamera::ProjectionMode::Orthographic )
									   {
										   float orthoSize = cc.Camera->GetOrthographicSize();
										   if ( Property("Size", orthoSize) )
											   cc.Camera->SetOrthographicSize(orthoSize);

										   float nearClip = cc.Camera->GetOrthographicNearClip();
										   if ( Property("Near Clip", nearClip) )
											   cc.Camera->SetOrthographicNearClip(nearClip);
										   ImGui::SameLine();
										   float farClip = cc.Camera->GetOrthographicFarClip();
										   if ( Property("Far Clip", farClip) )
											   cc.Camera->SetOrthographicFarClip(farClip);
									   }

									   EndPropertyGrid();
								   });

	DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](SpriteRendererComponent &mc)
										   {
										   });


	DrawComponent<ScriptComponent>("Script", entity, [=](ScriptComponent &sc) mutable
								   {
									   BeginPropertyGrid();

									   Property("Module Name", sc.ModuleName.c_str());
									   if ( ImGui::BeginDragDropTarget() )
									   {
										   if ( const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL") )
										   {
											   IM_ASSERT(payload->DataSize == sizeof(ScriptManager::Drop));
											   const auto drop = *static_cast<ScriptManager::Drop *>(payload->Data);

											   m_CachedNewModuleName = "Script." + drop.Path->stem().string();
											   if ( !ScriptEngine::ModuleExists(m_CachedNewModuleName) )
											   {
												   ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
												   ImGui::OpenPopup("Bad Script Module");
											   }
											   else
											   {
												   // Shutdown old script
												   if ( ScriptEngine::ModuleExists(sc.ModuleName) )
													   ScriptEngine::ShutdownScriptEntity(entity, sc.ModuleName);
												   sc.ChangeModule(std::move(m_CachedNewModuleName));
												   // Startup new script
												   if ( ScriptEngine::ModuleExists(sc.ModuleName) )
													   ScriptEngine::InitScriptEntity(entity);
											   }
											   delete drop.Path;
										   }
										   ImGui::EndDragDropTarget();
									   }

									   if ( ImGui::BeginPopupModal("Bad Script Module") )
									   {
										   ImGui::Text("Script module \"%s\" was not found in assembly", m_CachedNewModuleName.c_str());
										   if ( ImGui::Button("Close") )
										   {
											   m_CachedNewModuleName = "NONE";
											   ImGui::CloseCurrentPopup();
										   }
										   ImGui::EndPopup();
									   }

									   // Public Fields
									   if ( ScriptEngine::ModuleExists(sc.ModuleName) )
									   {
										   EntityInstanceData &entityInstanceData = ScriptEngine::GetEntityInstanceData(entity.GetSceneUUID(), id);
										   auto &moduleFieldMap = entityInstanceData.ModuleFieldMap;
										   if ( moduleFieldMap.find(sc.ModuleName) != moduleFieldMap.end() )
										   {
											   auto &publicFields = moduleFieldMap.at(sc.ModuleName);
											   for ( auto &[name, field] : publicFields )
											   {
												   const bool isRuntime = m_Context->m_IsPlaying && field.IsRuntimeAvailable();
												   switch ( field.Type )
												   {
												   case FieldType::Int:
												   {
													   int value = isRuntime ? field.GetRuntimeValue<int>() : field.GetStoredValue<int>();
													   if ( Property(field.Name.c_str(), value) )
													   {
														   if ( isRuntime )
															   field.SetRuntimeValue(value);
														   else
															   field.SetStoredValue(value);
													   }
													   break;
												   }
												   case FieldType::Float:
												   {
													   float value = isRuntime ? field.GetRuntimeValue<float>() : field.GetStoredValue<float>();
													   if ( Property(field.Name.c_str(), value, 0.2f) )
													   {
														   if ( isRuntime )
															   field.SetRuntimeValue(value);
														   else
															   field.SetStoredValue(value);
													   }
													   break;
												   }
												   case FieldType::Vec2:
												   {
													   glm::vec2 value = isRuntime ? field.GetRuntimeValue<glm::vec2>() : field.GetStoredValue<glm::vec2>();
													   if ( Property(field.Name.c_str(), value, 0.2f) )
													   {
														   if ( isRuntime )
															   field.SetRuntimeValue(value);
														   else
															   field.SetStoredValue(value);
													   }
													   break;
												   }
												   case FieldType::Vec3:
												   {
													   glm::vec3 value = isRuntime ? field.GetRuntimeValue<glm::vec3>() : field.GetStoredValue<glm::vec3>();
													   if ( Property(field.Name.c_str(), value, 0.2f) )
													   {
														   if ( isRuntime )
															   field.SetRuntimeValue(value);
														   else
															   field.SetStoredValue(value);
													   }
													   break;
												   }
												   case FieldType::Vec4:
												   {
													   glm::vec4 value = isRuntime ? field.GetRuntimeValue<glm::vec4>() : field.GetStoredValue<glm::vec4>();
													   if ( Property(field.Name.c_str(), value, 0.2f) )
													   {
														   if ( isRuntime )
															   field.SetRuntimeValue(value);
														   else
															   field.SetStoredValue(value);
													   }
													   break;
												   }
												   default:
													   break;
												   }
											   }
										   }
									   }

									   EndPropertyGrid();
								   });

	DrawComponent<RigidBody2DComponent>("Rigidbody 2D", entity, [](RigidBody2DComponent &rb2dc)
										{
											// Rigidbody2D Type
											const char *rb2dTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
											const char *currentType = rb2dTypeStrings[static_cast<int>(rb2dc.BodyType)];
											if ( ImGui::BeginCombo("Type", currentType) )
											{
												for ( int type = 0; type < 3; type++ )
												{
													const bool is_selected = (currentType == rb2dTypeStrings[type]);
													if ( ImGui::Selectable(rb2dTypeStrings[type], is_selected) )
													{
														currentType = rb2dTypeStrings[type];
														rb2dc.BodyType = static_cast<RigidBody2DComponent::Type>(type);
													}
													if ( is_selected )
														ImGui::SetItemDefaultFocus();
												}
												ImGui::EndCombo();
											}

											if ( rb2dc.BodyType == RigidBody2DComponent::Type::Dynamic )
											{
												BeginPropertyGrid();
												Property("Fixed Rotation", rb2dc.FixedRotation);
												EndPropertyGrid();
											}
										});

	DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](BoxCollider2DComponent &bc2dc)
										  {
											  BeginPropertyGrid();

											  Property("Offset", bc2dc.Offset);
											  Property("Size", bc2dc.Size);
											  Property("Density", bc2dc.Density);
											  Property("Friction", bc2dc.Friction);

											  EndPropertyGrid();
										  });

	DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](CircleCollider2DComponent &cc2dc)
											 {
												 BeginPropertyGrid();

												 Property("Offset", cc2dc.Offset);
												 Property("Radius", cc2dc.Radius);
												 Property("Density", cc2dc.Density);
												 Property("Friction", cc2dc.Friction);

												 EndPropertyGrid();
											 });
}
}