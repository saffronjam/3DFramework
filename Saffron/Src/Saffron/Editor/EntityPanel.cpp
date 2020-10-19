#include "SaffronPCH.h"

#include <assimp/scene.h>

#include "Saffron/Editor/ScriptPanel.h"
#include "Saffron/Core/Application.h"
#include "Saffron/Core/Math/SaffronMath.h"
#include "Saffron/Editor/EntityPanel.h"
#include "Saffron/Editor/ScriptPanel.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Script/ScriptEngine.h"


namespace Se
{

glm::mat4 Mat4FromAssimpMat4(const aiMatrix4x4 &matrix);

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
/// Entity Panel
///////////////////////////////////////////////////////////////////////////

EntityPanel::EntityPanel(const Ref<Scene> &context)
	:
	m_Context(context)
{
	m_TexStore["Checkerboard"] = Texture2D::Create("Assets/Editor/Checkerboard.tga");
}

void EntityPanel::OnGuiRender(const Ref<ScriptPanel> &scriptPanel)
{
	OnGuiRenderSceneHierarchy(scriptPanel);
	OnGuiRenderMaterial();
	OnGuiRenderMeshDebug();
}


void EntityPanel::SetContext(const Ref<Scene> &context)
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

void EntityPanel::SetSelected(Entity entity)
{
	m_SelectionContext = entity;
}

void EntityPanel::OnGuiRenderSceneHierarchy(const Ref<ScriptPanel> &scriptPanel)
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

			Gui::BeginPropertyGrid();
			Gui::Property("Name", entityName);
			Gui::Property("Mesh", meshComponent);
			Gui::Property("Script", scriptComponent);
			if ( scriptComponent )
			{
				ImGui::NextColumn();
				std::ostringstream oss;
				for ( const auto &scriptName : scriptPanel->GetScriptStats() ) { oss << scriptName.Class << '\0'; }
				oss << '\0';
				ImGui::Combo("##EntityCreateScriptComboOption", &scriptChosen, oss.str().c_str());
				ImGui::NextColumn();
			}
			Gui::Property("Camera", cameraComponent);
			Gui::Property("Sprite", spriteRendererComponent);
			Gui::Property("Rigid Body 2D", rigidBody2DComponent);
			Gui::Property("Box Collider 2D", boxCollider2DComponent);
			Gui::Property("Circle Collider 2D", circleCollider2DComponent);

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
						newEntity.AddComponent<ScriptComponent>(scriptPanel->GetScriptStats().at(scriptChosen).Full);
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

			Gui::InfoModal("Bad Entity Name", "Entity must have a name", badEntityName);

			Gui::EndPropertyGrid();

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
}

void EntityPanel::OnGuiRenderMaterial()
{
	ImGui::Begin("Materials");

	if ( m_SelectionContext )
	{
		Entity selectedEntity = m_SelectionContext;
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

					const ImGuiTreeNodeFlags node_flags = (selectedMaterialIndex == i ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_Leaf;
					const bool opened = ImGui::TreeNodeEx(static_cast<void *>(&materialInstance), node_flags, materialInstance->GetName().c_str());
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
									const std::filesystem::path filepath = Application::Get().OpenFile("");
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
									const std::filesystem::path filepath = Application::Get().OpenFile("");
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
									const std::filesystem::path filepath = Application::Get().OpenFile("");
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
									const std::filesystem::path filepath = Application::Get().OpenFile("");
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
}

void EntityPanel::OnGuiRenderMeshDebug()
{

	ImGui::Begin("Mesh Debug");
	if ( !m_SelectionContext )
	{
		ImGui::Text("No selected entity");
	}
	else if ( m_SelectionContext.HasComponent<MeshComponent>() )
	{
		Ref<Mesh> mesh = m_SelectionContext.GetComponent<MeshComponent>().Mesh;
		ImGui::TextWrapped("File: %s", mesh->GetFilepath().c_str());

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

void EntityPanel::DrawEntityNode(Entity entity)
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

void EntityPanel::DrawMeshNode(const Ref<Mesh> &mesh, UUID &entityUUID) const
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

void EntityPanel::MeshNodeHierarchy(const Ref<Mesh> &mesh,
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

void EntityPanel::DrawComponents(Entity entity)
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
									 Gui::BeginPropertyGrid();
									 Gui::Property("Name", mc.Mesh->GetFilepath().c_str());

									 if ( ImGui::BeginDragDropTarget() )
									 {
										 if ( const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ASSETMGR_DND") )
										 {
											 SE_CORE_ASSERT(payload->DataSize == sizeof(ScriptPanel::Drop));
											 const auto drop = *static_cast<ScriptPanel::Drop *>(payload->Data);
											 mc.Mesh = Ref<Mesh>::Create(drop.Stat->Path.string());
											 delete drop.Stat;
										 }
										 ImGui::EndDragDropTarget();
									 }

									 ImGui::NextColumn();
									 if ( ImGui::Button("Open...##openmesh") )
									 {
										 const fs::path filepath = Application::Get().OpenFile();
										 if ( !filepath.empty() )
											 mc.Mesh = Ref<Mesh>::Create(filepath.string());
									 }

									 Gui::EndPropertyGrid();
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

									   Gui::BeginPropertyGrid();
									   // Perspective parameters
									   if ( cc.Camera->GetProjectionMode() == SceneCamera::ProjectionMode::Perspective )
									   {
										   float verticalFOV = cc.Camera->GetPerspectiveVerticalFOV();
										   if ( Gui::Property("Vertical FOV", verticalFOV) )
											   cc.Camera->SetPerspectiveVerticalFOV(verticalFOV);

										   float nearClip = cc.Camera->GetPerspectiveNearClip();
										   if ( Gui::Property("Near Clip", nearClip) )
											   cc.Camera->SetPerspectiveNearClip(nearClip);
										   ImGui::SameLine();
										   float farClip = cc.Camera->GetPerspectiveFarClip();
										   if ( Gui::Property("Far Clip", farClip) )
											   cc.Camera->SetPerspectiveFarClip(farClip);
									   }

									   // Orthographic parameters
									   else if ( cc.Camera->GetProjectionMode() == SceneCamera::ProjectionMode::Orthographic )
									   {
										   float orthoSize = cc.Camera->GetOrthographicSize();
										   if ( Gui::Property("Size", orthoSize) )
											   cc.Camera->SetOrthographicSize(orthoSize);

										   float nearClip = cc.Camera->GetOrthographicNearClip();
										   if ( Gui::Property("Near Clip", nearClip) )
											   cc.Camera->SetOrthographicNearClip(nearClip);
										   ImGui::SameLine();
										   float farClip = cc.Camera->GetOrthographicFarClip();
										   if ( Gui::Property("Far Clip", farClip) )
											   cc.Camera->SetOrthographicFarClip(farClip);
									   }

									   Gui::EndPropertyGrid();
								   });

	DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](SpriteRendererComponent &mc)
										   {
										   });


	DrawComponent<ScriptComponent>("Script", entity, [=](ScriptComponent &sc) mutable
								   {
									   static std::string cachedNewModuleName;

									   Gui::BeginPropertyGrid();
									   Gui::Property("Module Name", sc.ModuleName.c_str());
									   if ( ImGui::BeginDragDropTarget() )
									   {
										   if ( const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("SCRIPTMGR_DND") )
										   {
											   SE_CORE_ASSERT(payload->DataSize == sizeof(ScriptPanel::Drop));
											   const auto drop = *static_cast<ScriptPanel::Drop *>(payload->Data);

											   cachedNewModuleName = "Script." + drop.Stat->Path.stem().string();
											   if ( !ScriptEngine::ModuleExists(cachedNewModuleName) )
											   {
												   ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
												   ImGui::OpenPopup("Bad Script Module");
											   }
											   else
											   {
												   // Shutdown old script
												   if ( ScriptEngine::ModuleExists(sc.ModuleName) )
													   ScriptEngine::ShutdownScriptEntity(entity, sc.ModuleName);
												   sc.ChangeModule(std::move(cachedNewModuleName));
												   // Startup new script
												   if ( ScriptEngine::ModuleExists(sc.ModuleName) )
													   ScriptEngine::InitScriptEntity(entity);
											   }
											   delete drop.Stat;
										   }
										   ImGui::EndDragDropTarget();
									   }

									   if ( ImGui::BeginPopupModal("Bad Script Module") )
									   {
										   ImGui::Text("Script module \"%s\" was not found in assembly", cachedNewModuleName.c_str());
										   if ( ImGui::Button("Close") )
										   {
											   cachedNewModuleName = "NONE";
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
													   if ( Gui::Property(field.Name, value) )
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
													   if ( Gui::Property(field.Name, value, 0.2f) )
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
													   if ( Gui::Property(field.Name, value, 0.2f) )
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
													   if ( Gui::Property(field.Name, value, 0.2f) )
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
													   if ( Gui::Property(field.Name, value, 0.2f) )
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

									   Gui::EndPropertyGrid();
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
												Gui::BeginPropertyGrid();
												Gui::Property("Fixed Rotation", rb2dc.FixedRotation);
												Gui::EndPropertyGrid();
											}
										});

	DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](BoxCollider2DComponent &bc2dc)
										  {
											  Gui::BeginPropertyGrid();

											  Gui::Property("Offset", bc2dc.Offset);
											  Gui::Property("Size", bc2dc.Size);
											  Gui::Property("Density", bc2dc.Density);
											  Gui::Property("Friction", bc2dc.Friction);

											  Gui::EndPropertyGrid();
										  });

	DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](CircleCollider2DComponent &cc2dc)
											 {
												 Gui::BeginPropertyGrid();

												 Gui::Property("Offset", cc2dc.Offset);
												 Gui::Property("Radius", cc2dc.Radius);
												 Gui::Property("Density", cc2dc.Density);
												 Gui::Property("Friction", cc2dc.Friction);

												 Gui::EndPropertyGrid();
											 });
}
}