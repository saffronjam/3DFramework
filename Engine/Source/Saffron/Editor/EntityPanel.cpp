#include "SaffronPCH.h"

#include <assimp/scene.h>

#include "Saffron/Core/Misc.h"
#include "Saffron/Math/SaffronMath.h"
#include "Saffron/Editor/EntityPanel.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Scene/RuntimeScene.h"
#include "Saffron/Script/ScriptEngine.h"


namespace Se
{

///////////////////////////////////////////////////////////////////////////
/// Helper functions
///////////////////////////////////////////////////////////////////////////

template<typename T, typename UIFunction>
static void DrawComponent(const String &name, Entity entity, UIFunction uiFunction)
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

EntityPanel::EntityPanel(const Shared<Scene> &context)
	:
	m_Context(context)
{
	m_TexStore["Checkerboard"] = Factory::Create<Texture2D>("Editor/Checkerboard.tga");
}

void EntityPanel::OnGuiRender()
{
	OnGuiRenderProperties();
	OnGuiRenderMaterial();
	OnGuiRenderMeshDebug();
}


void EntityPanel::SetContext(const Shared<Scene> &context)
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

void EntityPanel::SetSelectedEntity(Entity entity)
{
	m_SelectionContext = entity;
}

void EntityPanel::OnGuiRenderProperties()
{
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
					const String defaultMeshPath = "Cube1m.fbx";
					m_SelectionContext.AddComponent<MeshComponent>(CreateShared<Mesh>(defaultMeshPath));
					ImGui::CloseCurrentPopup();
				}
			}
			if ( !m_SelectionContext.HasComponent<DirectionalLightComponent>() )
			{
				if ( ImGui::Button("Directional Light") )
				{
					m_SelectionContext.AddComponent<DirectionalLightComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if ( !m_SelectionContext.HasComponent<SkylightComponent>() )
			{
				if ( ImGui::Button("Skylight") )
				{
					m_SelectionContext.AddComponent<SkylightComponent>(SceneEnvironment::Load("birchwood_4k.hdr"));
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
			if ( !m_SelectionContext.HasComponent<RigidBody3DComponent>() )
			{
				if ( ImGui::Button("Rigidbody 3D") )
				{
					m_SelectionContext.AddComponent<RigidBody3DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if ( !m_SelectionContext.HasComponent<BoxCollider3DComponent>() )
			{
				if ( ImGui::Button("Box Collider 3D") )
				{
					m_SelectionContext.AddComponent<BoxCollider3DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if ( !m_SelectionContext.HasComponent<SphereCollider3DComponent>() )
			{
				if ( ImGui::Button("Sphere Collider 3D") )
				{
					m_SelectionContext.AddComponent<SphereCollider3DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
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
			Shared<Mesh> mesh = selectedEntity.GetComponent<MeshComponent>().Mesh;
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

							auto &albedoColor = materialInstance->Get<Vector3f>("u_AlbedoColor");
							bool useAlbedoMap = materialInstance->Get<float>("u_AlbedoTexToggle");
							Shared<Texture2D> albedoMap = materialInstance->TryGetResource<Texture2D>("u_AlbedoTexture");
							ImGui::Image(albedoMap ? reinterpret_cast<void *>(albedoMap->GetRendererID()) : reinterpret_cast<void *>(m_TexStore["Checkerboard"]->GetRendererID()), ImVec2(64, 64));
							ImGui::PopStyleVar();
							if ( ImGui::IsItemHovered() )
							{
								if ( albedoMap )
								{
									ImGui::BeginTooltip();
									ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
									ImGui::TextUnformatted(albedoMap->GetFilepath().string().c_str());
									ImGui::PopTextWrapPos();
									ImGui::Image(reinterpret_cast<void *>(albedoMap->GetRendererID()), ImVec2(384, 384));
									ImGui::EndTooltip();
								}
								if ( ImGui::IsItemClicked() )
								{
									const Filepath filepath = FileIOManager::OpenFile();
									if ( !filepath.empty() )
									{
										albedoMap = Factory::Create<Texture2D>(filepath.string(), true/*m_AlbedoInput.sRGB*/);
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
									m_AlbedoInput.TextureMap = Factory::Create<Texture2D>(m_AlbedoInput.TextureMap->GetPath(), m_AlbedoInput.sRGB);
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
							Shared<Texture2D> normalMap = materialInstance->TryGetResource<Texture2D>("u_NormalTexture");
							ImGui::Image(normalMap ? reinterpret_cast<void *>(normalMap->GetRendererID()) : reinterpret_cast<void *>(m_TexStore["Checkerboard"]->GetRendererID()), ImVec2(64, 64));
							ImGui::PopStyleVar();
							if ( ImGui::IsItemHovered() )
							{
								if ( normalMap )
								{
									ImGui::BeginTooltip();
									ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
									ImGui::TextUnformatted(normalMap->GetFilepath().string().c_str());
									ImGui::PopTextWrapPos();
									ImGui::Image(reinterpret_cast<void *>(normalMap->GetRendererID()), ImVec2(384, 384));
									ImGui::EndTooltip();
								}
								if ( ImGui::IsItemClicked() )
								{
									const Filepath filepath = FileIOManager::OpenFile();
									if ( !filepath.empty() )
									{
										normalMap = Factory::Create<Texture2D>(filepath.string());
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
							Shared<Texture2D> metalnessMap = materialInstance->TryGetResource<Texture2D>("u_MetalnessTexture");
							ImGui::Image(metalnessMap ? reinterpret_cast<void *>(metalnessMap->GetRendererID()) : reinterpret_cast<void *>(m_TexStore["Checkerboard"]->GetRendererID()), ImVec2(64, 64));
							ImGui::PopStyleVar();
							if ( ImGui::IsItemHovered() )
							{
								if ( metalnessMap )
								{
									ImGui::BeginTooltip();
									ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
									ImGui::TextUnformatted(metalnessMap->GetFilepath().string().c_str());
									ImGui::PopTextWrapPos();
									ImGui::Image(reinterpret_cast<void *>(metalnessMap->GetRendererID()), ImVec2(384, 384));
									ImGui::EndTooltip();
								}
								if ( ImGui::IsItemClicked() )
								{
									const Filepath filepath = FileIOManager::OpenFile();
									if ( !filepath.empty() )
									{
										metalnessMap = Factory::Create<Texture2D>(filepath.string());
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
							Shared<Texture2D> roughnessMap = materialInstance->TryGetResource<Texture2D>("u_RoughnessTexture");
							ImGui::Image(roughnessMap ? reinterpret_cast<void *>(roughnessMap->GetRendererID()) : reinterpret_cast<void *>(m_TexStore["Checkerboard"]->GetRendererID()), ImVec2(64, 64));
							ImGui::PopStyleVar();
							if ( ImGui::IsItemHovered() )
							{
								if ( roughnessMap )
								{
									ImGui::BeginTooltip();
									ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
									ImGui::TextUnformatted(roughnessMap->GetFilepath().string().c_str());
									ImGui::PopTextWrapPos();
									ImGui::Image(reinterpret_cast<void *>(roughnessMap->GetRendererID()), ImVec2(384, 384));
									ImGui::EndTooltip();
								}
								if ( ImGui::IsItemClicked() )
								{
									const Filepath filepath = FileIOManager::OpenFile();
									if ( !filepath.empty() )
									{
										roughnessMap = Factory::Create<Texture2D>(filepath.string());
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
		Shared<Mesh> mesh = m_SelectionContext.GetComponent<MeshComponent>().Mesh;
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

void EntityPanel::DrawComponents(Entity entity)
{
	ImGui::AlignTextToFramePadding();

	const auto id = entity.GetComponent<IDComponent>().ID;

	if ( entity.HasComponent<TagComponent>() )
	{
		if ( ImGui::TreeNodeEx(reinterpret_cast<void *>(static_cast<Uint32>(entity) | typeid(TagComponent).hash_code()), ImGuiTreeNodeFlags_DefaultOpen, "Identifiers") )
		{
			auto &tag = entity.GetComponent<TagComponent>().Tag;
			Gui::BeginPropertyGrid();
			// ID
			Gui::Property("ID", std::to_string(static_cast<Uint64>(id)));
			Gui::Property("Tag", tag);
			Gui::EndPropertyGrid();
			ImGui::TreePop();
		}
		ImGui::Separator();
	}

	if ( entity.HasComponent<TransformComponent>() )
	{
		auto &tc = entity.GetComponent<TransformComponent>();
		if ( ImGui::TreeNodeEx(reinterpret_cast<void *>(static_cast<Uint32>(entity) | typeid(TransformComponent).hash_code()), ImGuiTreeNodeFlags_DefaultOpen, "Transform") )
		{

			Gui::BeginPropertyGrid();

			auto dc = Misc::GetTransformDecomposition(tc);
			Vector3f rotation = glm::degrees(glm::eulerAngles(dc.Rotation));

			bool updateTransform = false;
			if ( Gui::Property("Translation", dc.Translation, 0.0f, 0.0f, 0.05f, Gui::PropertyFlag::Drag, [&dc, &updateTransform] {dc.Translation = Vector3f(0.0f); updateTransform = true; }) )
			{
				updateTransform = true;
			}

			if ( Gui::Property("Rotation", rotation, 0.0f, 0.0f, 0.5f, Gui::PropertyFlag::Drag, [&rotation, &updateTransform] {rotation = Vector3f(0.0f); updateTransform = true; }) )
			{
				updateTransform = true;
			}

			if ( Gui::Property("Scale", dc.Scale, 0.15f, 0.0f, 0.15f, Gui::PropertyFlag::Drag, [&dc, &updateTransform] {dc.Scale = Vector3f(1.0f, 1.0f, 1.0f); updateTransform = true; }) )
			{
				Misc::LowClamp(dc.Scale.x, 0.15f);
				Misc::LowClamp(dc.Scale.y, 0.15f);
				Misc::LowClamp(dc.Scale.z, 0.15f);
				updateTransform = true;
			}

			Gui::EndPropertyGrid();

			if ( updateTransform )
			{
				tc.Transform = glm::translate(dc.Translation) *
					glm::toMat4(glm::quat(glm::radians(rotation))) *
					glm::scale(dc.Scale);
			}
			ImGui::TreePop();
		}
		ImGui::Separator();
	}

	DrawComponent<MeshComponent>("Mesh", entity, [](MeshComponent &mc)
								 {
									 Gui::BeginPropertyGrid();
									 Gui::Property("Name", mc.Mesh->GetFilepath().string(), "Open...", [&]
												   {
													   const Filepath filepath = FileIOManager::OpenFile();
													   if ( !filepath.empty() )
														   mc.Mesh = CreateShared<Mesh>(filepath.string());
												   });

									 if ( ImGui::BeginDragDropTarget() )
									 {
										 if ( const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ASSETMGR_DND") )
										 {
											 SE_CORE_ASSERT(payload->DataSize == sizeof(ScriptPanel::Drop));
											 const auto drop = *static_cast<ScriptPanel::Drop *>(payload->Data);
											 mc.Mesh = CreateShared<Mesh>(drop.Stat->Path.string());
											 delete drop.Stat;
										 }
										 ImGui::EndDragDropTarget();
									 }

									 const auto &transform = mc.Mesh->GetLocalTransform();
									 auto dc = Misc::GetTransformDecomposition(transform);
									 Vector3f rotation = glm::degrees(glm::eulerAngles(dc.Rotation));

									 bool updateTransform = false;
									 if ( Gui::Property("Translation", dc.Translation, 0.0f, 0.0f, 0.05f, Gui::PropertyFlag::Drag, [&dc, &updateTransform] {dc.Translation = Vector3f(0.0f); updateTransform = true; }) )
									 {
										 updateTransform = true;
									 }
									 if ( Gui::Property("Rotation", rotation, 0.0f, 0.0f, 0.5f, Gui::PropertyFlag::Drag, [&rotation, &updateTransform] {rotation = Vector3f(0.0f); updateTransform = true; }) )
									 {
										 updateTransform = true;
									 }
									 if ( Gui::Property("Scale", dc.Scale, 0.0f, 0.0f, 0.15f, Gui::PropertyFlag::Drag, [&dc, &updateTransform] {dc.Scale = Vector3f(1.0f); updateTransform = true; }) )
									 {
										 if ( dc.Scale.x > 0.0f && dc.Scale.y > 0.0f && dc.Scale.z > 0.0f )
											 updateTransform = true;
									 }

									 Gui::EndPropertyGrid();

									 if ( updateTransform )
									 {
										 const Matrix4f newTransform = glm::translate(dc.Translation) *
											 glm::toMat4(glm::quat(glm::radians(rotation))) *
											 glm::scale(dc.Scale);
										 mc.Mesh->SetLocalTransform(newTransform);
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
											   const bool is_selected = currentProj == projTypeStrings[type];
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

									   Gui::Property("Mesh", cc.DrawMesh);
									   Gui::Property("Frustum", cc.DrawFrustum);

									   const auto &transform = cc.CameraMesh->GetLocalTransform();
									   auto dc = Misc::GetTransformDecomposition(transform);
									   Vector3f rotation = glm::degrees(glm::eulerAngles(dc.Rotation));

									   bool updateTransform = false;
									   if ( Gui::Property("Translation", dc.Translation, 0.0f, 0.0f, 0.05f, Gui::PropertyFlag::Drag, [&dc, &updateTransform] {dc.Translation = Vector3f(0.0f); updateTransform = true; }) )
									   {
										   updateTransform = true;
									   }
									   if ( Gui::Property("Rotation", rotation, 0.0f, 0.0f, 0.5f, Gui::PropertyFlag::Drag, [&rotation, &updateTransform] {rotation = Vector3f(0.0f); updateTransform = true; }) )
									   {
										   updateTransform = true;
									   }
									   if ( Gui::Property("Scale", dc.Scale, 0.0f, 0.0f, 0.15f, Gui::PropertyFlag::Drag, [&dc, &updateTransform] {dc.Scale = Vector3f(1.0f); updateTransform = true; }) )
									   {
										   if ( dc.Scale.x > 0.0f && dc.Scale.y > 0.0f && dc.Scale.z > 0.0f )
											   updateTransform = true;
									   }

									   Gui::EndPropertyGrid();

									   if ( updateTransform )
									   {
										   const Matrix4f newTransform = glm::translate(dc.Translation) *
											   glm::toMat4(glm::quat(glm::radians(rotation))) *
											   glm::scale(dc.Scale);
										   cc.CameraMesh->SetLocalTransform(newTransform);
									   }
								   });

	DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](SpriteRendererComponent &mc)
										   {
										   });

	DrawComponent<DirectionalLightComponent>("Directional Light", entity, [](DirectionalLightComponent &dlc)
											 {
												 Gui::BeginPropertyGrid();
												 Gui::Property("Radiance", dlc.Radiance, Gui::PropertyFlag::Color);
												 Gui::Property("Intensity", dlc.Intensity);
												 Gui::Property("Cast Shadows", dlc.CastShadows);
												 Gui::Property("Soft Shadows", dlc.SoftShadows);
												 Gui::Property("Source Size", dlc.LightSize);
												 Gui::EndPropertyGrid();
											 });

	DrawComponent<SkylightComponent>("Skylight", entity, [](SkylightComponent &slc)
									 {
										 ImGui::Columns(3);
										 ImGui::SetColumnWidth(0, 100);
										 ImGui::SetColumnWidth(1, 300);
										 ImGui::SetColumnWidth(2, 40);
										 ImGui::Text("File Path");
										 ImGui::NextColumn();
										 ImGui::PushItemWidth(-1);
										 if ( !slc.SceneEnvironment->GetFilepath().empty() )
										 {
											 ImGui::InputText("##envfilepath", (char *)slc.SceneEnvironment->GetFilepath().c_str(), 256, ImGuiInputTextFlags_ReadOnly);
										 }
										 else
										 {
											 ImGui::InputText("##envfilepath", (char *)"Empty", 256, ImGuiInputTextFlags_ReadOnly);
										 }
										 ImGui::PopItemWidth();
										 ImGui::NextColumn();
										 if ( ImGui::Button("...##openenv") )
										 {
											 const Filepath filepath = FileIOManager::OpenFile({ "*.hdr", { "*.hdr" } });
											 if ( !filepath.empty() )
											 {
												 slc.SceneEnvironment = SceneEnvironment::Load(filepath);
											 }
										 }
										 ImGui::Columns(1);

										 Gui::BeginPropertyGrid();
										 Gui::Property("Intensity", slc.Intensity, 0.01f, 0.0f, 5.0f);
										 Gui::EndPropertyGrid();
									 });



	DrawComponent<ScriptComponent>("Script", entity, [=](ScriptComponent &sc) mutable
								   {
									   static String cachedNewModuleName;

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
												   sc.ChangeModule(Move(cachedNewModuleName));
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
												   const bool isRuntime = std::dynamic_pointer_cast<RuntimeScene>(m_Context) && field.IsRuntimeAvailable();
												   switch ( field.Type )
												   {
												   case FieldType::Int:
												   {
													   int value = isRuntime ? field.GetRuntimeValue<int>() : field.GetStoredValue<int>();
													   if ( Gui::Property(field.Name, value, 0, 0, 1, Gui::PropertyFlag::Drag) )
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
													   if ( Gui::Property(field.Name, value, 0.0f, 0.0f, 0.05f, Gui::PropertyFlag::Drag) )
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
													   Vector2f value = isRuntime ? field.GetRuntimeValue<Vector2f>() : field.GetStoredValue<Vector2f>();
													   if ( Gui::Property(field.Name, value, 0.0f, 0.0f, 0.05f, Gui::PropertyFlag::Drag) )
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
													   Vector3f value = isRuntime ? field.GetRuntimeValue<Vector3f>() : field.GetStoredValue<Vector3f>();
													   if ( Gui::Property(field.Name, value, 0.0f, 0.0f, 0.05f, Gui::PropertyFlag::Drag) )
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
													   Vector4f value = isRuntime ? field.GetRuntimeValue<Vector4f>() : field.GetStoredValue<Vector4f>();
													   if ( Gui::Property(field.Name, value, 0.0f, 0.0f, 0.05f, Gui::PropertyFlag::Drag) )
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
													const bool is_selected = currentType == rb2dTypeStrings[type];
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
											  Gui::Property("Translation", bc2dc.Offset, 0.0f, 0.0f, 0.05f, Gui::PropertyFlag::Drag);
											  Gui::Property("Offset", bc2dc.Offset, 0.0f, 0.0f, 0.05f, Gui::PropertyFlag::Drag);
											  Gui::Property("Size", bc2dc.Size, 0.0f, 0.0f, 0.05f, Gui::PropertyFlag::Drag);
											  Gui::Property("Density", bc2dc.Density, 0.0f, 0.0f, 0.05f, Gui::PropertyFlag::Drag);
											  Gui::Property("Friction", bc2dc.Friction, 0.0f, 0.0f, 0.05f, Gui::PropertyFlag::Drag);
											  Gui::Property("Bounding", bc2dc.DrawBounding);
											  Gui::EndPropertyGrid();
										  });

	DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](CircleCollider2DComponent &cc2dc)
											 {
												 Gui::BeginPropertyGrid();
												 Gui::Property("Offset", cc2dc.Offset, 0.0f, 0.0f, 0.05f, Gui::PropertyFlag::Drag);
												 Gui::Property("Radius", cc2dc.Radius, 0.0f, 0.0f, 0.05f, Gui::PropertyFlag::Drag);
												 Gui::Property("Density", cc2dc.Density, 0.0f, 0.0f, 0.05f, Gui::PropertyFlag::Drag);
												 Gui::Property("Friction", cc2dc.Friction, 0.0f, 0.0f, 0.05f, Gui::PropertyFlag::Drag);
												 Gui::Property("Bounding", cc2dc.DrawBounding);
												 Gui::EndPropertyGrid();
											 });
	DrawComponent<RigidBody3DComponent>("Rigidbody 3D", entity, [](RigidBody3DComponent &rb3dc)
										{
											// Rigidbody2D Type
											const char *rb3dTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
											const char *currentType = rb3dTypeStrings[static_cast<int>(rb3dc.BodyType)];
											if ( ImGui::BeginCombo("Type", currentType) )
											{
												for ( int type = 0; type < 3; type++ )
												{
													const bool is_selected = currentType == rb3dTypeStrings[type];
													if ( ImGui::Selectable(rb3dTypeStrings[type], is_selected) )
													{
														currentType = rb3dTypeStrings[type];
														rb3dc.BodyType = static_cast<RigidBody3DComponent::Type>(type);
													}
													if ( is_selected )
														ImGui::SetItemDefaultFocus();
												}
												ImGui::EndCombo();
											}
										});

	DrawComponent<BoxCollider3DComponent>("Box Collider 3D", entity, [](BoxCollider3DComponent &bc3dc)
										  {
											  Gui::BeginPropertyGrid();
											  Gui::Property("Offset", bc3dc.Offset, 0.0f, 0.0f, 0.05f, Gui::PropertyFlag::Drag);
											  Gui::Property("Size", bc3dc.Size, 0.0f, 0.0f, 0.05f, Gui::PropertyFlag::Drag);
											  Gui::Property("Density", bc3dc.Density, 0.0f, 0.0f, 0.05f, Gui::PropertyFlag::Drag);
											  Gui::Property("Friction", bc3dc.Friction, 0.0f, 0.0f, 0.05f, Gui::PropertyFlag::Drag);
											  Gui::Property("Bounding", bc3dc.DrawBounding);
											  Gui::EndPropertyGrid();
										  });

	DrawComponent<SphereCollider3DComponent>("Circle Collider 3D", entity, [](SphereCollider3DComponent &cc3dc)
											 {
												 Gui::BeginPropertyGrid();
												 Gui::Property("Offset", cc3dc.Offset, 0.0f, 0.0f, 0.05f, Gui::PropertyFlag::Drag);
												 Gui::Property("Radius", cc3dc.Radius, 0.0f, 0.0f, 0.05f, Gui::PropertyFlag::Drag);
												 Gui::Property("Density", cc3dc.Density, 0.0f, 0.0f, 0.05f, Gui::PropertyFlag::Drag);
												 Gui::Property("Friction", cc3dc.Friction, 0.0f, 0.0f, 0.05f, Gui::PropertyFlag::Drag);
												 Gui::Property("Bounding", cc3dc.DrawBounding);
												 Gui::EndPropertyGrid();
											 });
}
}