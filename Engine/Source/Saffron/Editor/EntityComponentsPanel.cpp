#include "SaffronPCH.h"

#include <assimp/scene.h>

#include "Saffron/Core/Misc.h"
#include "Saffron/Math/SaffronMath.h"
#include "Saffron/Editor/EntityComponentsPanel.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Scene/RuntimeScene.h"
#include "Saffron/Script/ScriptEngine.h"


namespace Se
{
///////////////////////////////////////////////////////////////////////////
/// Helper functions
///////////////////////////////////////////////////////////////////////////

template <typename T, typename UIFunction>
static void DrawComponent(const String& name, Entity entity, UIFunction uiFunction)
{
	if (entity.HasComponent<T>())
	{
		bool removeComponent = false;

		auto& component = entity.GetComponent<T>();
		const bool open = ImGui::TreeNodeEx(
			reinterpret_cast<void*>(static_cast<Uint32>(entity) | typeid(T).hash_code()),
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap, name.c_str());
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
		if (ImGui::Button("+"))
		{
			ImGui::OpenPopup("ComponentSettings");
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		if (ImGui::BeginPopup("ComponentSettings"))
		{
			if (ImGui::MenuItem("Remove component")) removeComponent = true;

			ImGui::EndPopup();
		}

		if (open)
		{
			uiFunction(component);
			ImGui::NextColumn();
			ImGui::Columns(1);
			ImGui::TreePop();
		}
		ImGui::Separator();

		if (removeComponent) entity.RemoveComponent<T>();
	}
}

///////////////////////////////////////////////////////////////////////////
/// Entity Panel
///////////////////////////////////////////////////////////////////////////

EntityComponentsPanel::EntityComponentsPanel(const Shared<Scene>& context) :
	_context(context)
{
	_texStore["Checkerboard"] = Texture2D::Create("Editor/Checkerboard.tga");
}

void EntityComponentsPanel::OnGuiRender()
{
	OnGuiRenderProperties();
	OnGuiRenderMaterial();
	OnGuiRenderMeshDebug();
}


void EntityComponentsPanel::SetContext(const Shared<Scene>& context)
{
	_context = context;
	if (_selectionContext)
	{
		// Try and find same entity in new scene
		const auto& entityMap = _context->GetEntityMap();
		const UUID selectedEntityUUID = _selectionContext.GetUUID();
		if (entityMap.find(selectedEntityUUID) != entityMap.end()) _selectionContext = entityMap.
			at(selectedEntityUUID);
	}
}

void EntityComponentsPanel::SetSelectedEntity(Entity entity)
{
	_selectionContext = entity;
}

void EntityComponentsPanel::OnGuiRenderProperties()
{
	ImGui::Begin("Properties");
	if (_selectionContext)
	{
		DrawComponents(_selectionContext);

		if (ImGui::Button("Add Component")) ImGui::OpenPopup("AddComponentPanel");

		if (ImGui::BeginPopup("AddComponentPanel"))
		{
			if (!_selectionContext.HasComponent<CameraComponent>())
			{
				if (ImGui::Button("Camera"))
				{
					_selectionContext.AddComponent<CameraComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!_selectionContext.HasComponent<MeshComponent>())
			{
				if (ImGui::Button("Mesh"))
				{
					const String defaultMeshPath = "Cube1m.fbx";
					_selectionContext.AddComponent<MeshComponent>(Shared<Mesh>::Create(defaultMeshPath));
					ImGui::CloseCurrentPopup();
				}
			}
			if (!_selectionContext.HasComponent<DirectionalLightComponent>())
			{
				if (ImGui::Button("Directional Light"))
				{
					_selectionContext.AddComponent<DirectionalLightComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!_selectionContext.HasComponent<SkylightComponent>())
			{
				if (ImGui::Button("Skylight"))
				{
					_selectionContext.AddComponent<SkylightComponent>(SceneEnvironment::Load("birchwood_4k.hdr"));
					ImGui::CloseCurrentPopup();
				}
			}
			if (!_selectionContext.HasComponent<ScriptComponent>())
			{
				if (ImGui::Button("Script"))
				{
					_selectionContext.AddComponent<ScriptComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!_selectionContext.HasComponent<SpriteRendererComponent>())
			{
				if (ImGui::Button("Sprite Renderer"))
				{
					_selectionContext.AddComponent<SpriteRendererComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!_selectionContext.HasComponent<RigidBody2DComponent>())
			{
				if (ImGui::Button("Rigidbody 2D"))
				{
					_selectionContext.AddComponent<RigidBody2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!_selectionContext.HasComponent<BoxCollider2DComponent>())
			{
				if (ImGui::Button("Box Collider 2D"))
				{
					_selectionContext.AddComponent<BoxCollider2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!_selectionContext.HasComponent<CircleCollider2DComponent>())
			{
				if (ImGui::Button("Circle Collider 2D"))
				{
					_selectionContext.AddComponent<CircleCollider2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!_selectionContext.HasComponent<RigidBody3DComponent>())
			{
				if (ImGui::Button("Rigidbody 3D"))
				{
					_selectionContext.AddComponent<RigidBody3DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!_selectionContext.HasComponent<BoxCollider3DComponent>())
			{
				if (ImGui::Button("Box Collider 3D"))
				{
					_selectionContext.AddComponent<BoxCollider3DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!_selectionContext.HasComponent<SphereCollider3DComponent>())
			{
				if (ImGui::Button("Sphere Collider 3D"))
				{
					_selectionContext.AddComponent<SphereCollider3DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
	}
	ImGui::End();
}

void EntityComponentsPanel::OnGuiRenderMaterial()
{
	ImGui::Begin("Materials");

	if (_selectionContext)
	{
		Entity selectedEntity = _selectionContext;
		if (selectedEntity.HasComponent<MeshComponent>())
		{
			Shared<Mesh> mesh = selectedEntity.GetComponent<MeshComponent>().Mesh;
			if (mesh)
			{
				auto materials = mesh->GetMaterials();
				static uint32_t selectedMaterialIndex = 0;
				for (uint32_t i = 0; i < materials.size(); i++)
				{
					auto& materialInstance = materials[i];

					const ImGuiTreeNodeFlags node_flags = (selectedMaterialIndex == i ? ImGuiTreeNodeFlags_Selected : 0)
						| ImGuiTreeNodeFlags_Leaf;
					const bool opened = ImGui::TreeNodeEx(static_cast<void*>(&materialInstance), node_flags,
					                                      materialInstance->GetName().c_str());
					if (ImGui::IsItemClicked())
					{
						selectedMaterialIndex = i;
					}
					if (opened) ImGui::TreePop();
				}

				ImGui::Separator();

				// Selected material
				if (selectedMaterialIndex < materials.size())
				{
					auto& materialInstance = materials[selectedMaterialIndex];
					ImGui::Text("Shader: %s", materialInstance->GetShader()->GetName().c_str());
					// Textures ------------------------------------------------------------------------------
					{
						// Albedo
						if (ImGui::CollapsingHeader("Albedo", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
						{
							ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));

							auto& albedoColor = materialInstance->Get<Vector3f>("u_AlbedoColor");
							bool useAlbedoMap = materialInstance->Get<float>("u_AlbedoTexToggle");
							auto albedoMap = materialInstance->TryGetResource<Texture2D>("u_AlbedoTexture");
							ImGui::Image(
								albedoMap
									? reinterpret_cast<void*>(albedoMap->GetRendererID())
									: reinterpret_cast<void*>(_texStore["Checkerboard"]->GetRendererID()),
								ImVec2(64, 64));
							ImGui::PopStyleVar();
							if (ImGui::IsItemHovered())
							{
								if (albedoMap)
								{
									ImGui::BeginTooltip();
									ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
									ImGui::TextUnformatted(albedoMap->GetFilepath().string().c_str());
									ImGui::PopTextWrapPos();
									ImGui::Image(reinterpret_cast<void*>(albedoMap->GetRendererID()), ImVec2(384, 384));
									ImGui::EndTooltip();
								}
								if (ImGui::IsItemClicked())
								{
									const Filepath filepath = FileIOManager::OpenFile();
									if (!filepath.empty())
									{
										albedoMap = Texture2D::Create(filepath.string(), true/*_albedoInput.sRGB*/);
										materialInstance->Set("u_AlbedoTexture", albedoMap);
									}
								}
							}
							ImGui::SameLine();
							ImGui::BeginGroup();
							if (ImGui::Checkbox("Use##AlbedoMap", &useAlbedoMap)) materialInstance->Set<float>(
								"u_AlbedoTexToggle", useAlbedoMap ? 1.0f : 0.0f);

							/*if (ImGui::Checkbox("sRGB##AlbedoMap", &_albedoInput.sRGB))
							{
								if (_albedoInput.TextureMap)
									_albedoInput.TextureMap = Shared<Texture2D>(_albedoInput.TextureMap->::CreateGetPath(), _albedoInput.sRGB);
							}*/
							ImGui::EndGroup();
							ImGui::SameLine();
							ImGui::ColorEdit3("Color##Albedo", value_ptr(albedoColor), ImGuiColorEditFlags_NoInputs);
						}
					}
					{
						// Normals
						if (ImGui::CollapsingHeader("Normals", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
						{
							ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
							bool useNormalMap = materialInstance->Get<float>("u_NormalTexToggle");
							auto normalMap = materialInstance->TryGetResource<Texture2D>("u_NormalTexture");
							ImGui::Image(
								normalMap
									? reinterpret_cast<void*>(normalMap->GetRendererID())
									: reinterpret_cast<void*>(_texStore["Checkerboard"]->GetRendererID()),
								ImVec2(64, 64));
							ImGui::PopStyleVar();
							if (ImGui::IsItemHovered())
							{
								if (normalMap)
								{
									ImGui::BeginTooltip();
									ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
									ImGui::TextUnformatted(normalMap->GetFilepath().string().c_str());
									ImGui::PopTextWrapPos();
									ImGui::Image(reinterpret_cast<void*>(normalMap->GetRendererID()), ImVec2(384, 384));
									ImGui::EndTooltip();
								}
								if (ImGui::IsItemClicked())
								{
									const Filepath filepath = FileIOManager::OpenFile();
									if (!filepath.empty())
									{
										normalMap = Texture2D::Create(filepath.string());
										materialInstance->Set("u_NormalTexture", normalMap);
									}
								}
							}
							ImGui::SameLine();
							if (ImGui::Checkbox("Use##NormalMap", &useNormalMap)) materialInstance->Set<float>(
								"u_NormalTexToggle", useNormalMap ? 1.0f : 0.0f);
						}
					}
					{
						// Metalness
						if (ImGui::CollapsingHeader("Metalness", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
						{
							ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
							auto& metalnessValue = materialInstance->Get<float>("u_Metalness");
							bool useMetalnessMap = materialInstance->Get<float>("u_MetalnessTexToggle");
							auto metalnessMap = materialInstance->TryGetResource<Texture2D>("u_MetalnessTexture");
							ImGui::Image(
								metalnessMap
									? reinterpret_cast<void*>(metalnessMap->GetRendererID())
									: reinterpret_cast<void*>(_texStore["Checkerboard"]->GetRendererID()),
								ImVec2(64, 64));
							ImGui::PopStyleVar();
							if (ImGui::IsItemHovered())
							{
								if (metalnessMap)
								{
									ImGui::BeginTooltip();
									ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
									ImGui::TextUnformatted(metalnessMap->GetFilepath().string().c_str());
									ImGui::PopTextWrapPos();
									ImGui::Image(reinterpret_cast<void*>(metalnessMap->GetRendererID()),
									             ImVec2(384, 384));
									ImGui::EndTooltip();
								}
								if (ImGui::IsItemClicked())
								{
									const Filepath filepath = FileIOManager::OpenFile();
									if (!filepath.empty())
									{
										metalnessMap = Texture2D::Create(filepath.string());
										materialInstance->Set("u_MetalnessTexture", metalnessMap);
									}
								}
							}
							ImGui::SameLine();
							if (ImGui::Checkbox("Use##MetalnessMap", &useMetalnessMap)) materialInstance->Set<float>(
								"u_MetalnessTexToggle", useMetalnessMap ? 1.0f : 0.0f);
							ImGui::SameLine();
							ImGui::SliderFloat("Value##MetalnessInput", &metalnessValue, 0.0f, 1.0f);
						}
					}
					{
						// Roughness
						if (ImGui::CollapsingHeader("Roughness", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
						{
							ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
							auto& roughnessValue = materialInstance->Get<float>("u_Roughness");
							bool useRoughnessMap = materialInstance->Get<float>("u_RoughnessTexToggle");
							auto roughnessMap = materialInstance->TryGetResource<Texture2D>("u_RoughnessTexture");
							ImGui::Image(
								roughnessMap
									? reinterpret_cast<void*>(roughnessMap->GetRendererID())
									: reinterpret_cast<void*>(_texStore["Checkerboard"]->GetRendererID()),
								ImVec2(64, 64));
							ImGui::PopStyleVar();
							if (ImGui::IsItemHovered())
							{
								if (roughnessMap)
								{
									ImGui::BeginTooltip();
									ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
									ImGui::TextUnformatted(roughnessMap->GetFilepath().string().c_str());
									ImGui::PopTextWrapPos();
									ImGui::Image(reinterpret_cast<void*>(roughnessMap->GetRendererID()),
									             ImVec2(384, 384));
									ImGui::EndTooltip();
								}
								if (ImGui::IsItemClicked())
								{
									const Filepath filepath = FileIOManager::OpenFile();
									if (!filepath.empty())
									{
										roughnessMap = Texture2D::Create(filepath.string());
										materialInstance->Set("u_RoughnessTexture", roughnessMap);
									}
								}
							}
							ImGui::SameLine();
							if (ImGui::Checkbox("Use##RoughnessMap", &useRoughnessMap)) materialInstance->Set<float>(
								"u_RoughnessTexToggle", useRoughnessMap ? 1.0f : 0.0f);
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

void EntityComponentsPanel::OnGuiRenderMeshDebug()
{
	ImGui::Begin("Mesh Debug");
	if (!_selectionContext)
	{
		ImGui::Text("No selected entity");
	}
	else if (_selectionContext.HasComponent<MeshComponent>())
	{
		Shared<Mesh> mesh = _selectionContext.GetComponent<MeshComponent>().Mesh;
		ImGui::TextWrapped("File: %s", mesh->GetFilepath().c_str());

		if (mesh->_isAnimated)
		{
			if (ImGui::CollapsingHeader("Animation"))
			{
				if (ImGui::Button(mesh->_animationPlaying ? "Pause" : "Play")) mesh->_animationPlaying = !mesh->
					_animationPlaying;

				ImGui::SliderFloat("##AnimationTime", &mesh->_animationTime, 0.0f,
				                   static_cast<float>(mesh->_scene->mAnimations[0]->mDuration));
				ImGui::DragFloat("Time Scale", &mesh->_timeMultiplier, 0.05f, 0.0f, 10.0f);
			}
		}
	}
	else
	{
		ImGui::Text("Selected Entity has no mesh");
	}
	ImGui::End();
}

void EntityComponentsPanel::DrawComponents(Entity entity)
{
	ImGui::AlignTextToFramePadding();

	auto id = entity.GetComponent<IDComponent>().ID;

	ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

	if (entity.HasComponent<TagComponent>())
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;
		char buffer[256];
		memset(buffer, 0, 256);
		memcpy(buffer, tag.c_str(), tag.length());
		ImGui::PushItemWidth(contentRegionAvailable.x * 0.5f);
		if (ImGui::InputText("##Tag", buffer, 256))
		{
			tag = std::string(buffer);
		}
		ImGui::PopItemWidth();
	}

	// ID
	ImGui::SameLine();
	ImGui::TextDisabled("%llx", id);
	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 textSize = ImGui::CalcTextSize("Add Component");
	ImGui::SameLine(contentRegionAvailable.x - (textSize.x + GImGui->Style.FramePadding.y));
	if (ImGui::Button("Add Component")) ImGui::OpenPopup("AddComponentPanel");

	if (ImGui::BeginPopup("AddComponentPanel"))
	{
		if (!_selectionContext.HasComponent<CameraComponent>())
		{
			if (ImGui::Button("Camera"))
			{
				_selectionContext.AddComponent<CameraComponent>();
				ImGui::CloseCurrentPopup();
			}
		}
		if (!_selectionContext.HasComponent<MeshComponent>())
		{
			if (ImGui::Button("Mesh"))
			{
				_selectionContext.AddComponent<MeshComponent>();
				ImGui::CloseCurrentPopup();
			}
		}
		if (!_selectionContext.HasComponent<DirectionalLightComponent>())
		{
			if (ImGui::Button("Directional Light"))
			{
				_selectionContext.AddComponent<DirectionalLightComponent>();
				ImGui::CloseCurrentPopup();
			}
		}
		if (!_selectionContext.HasComponent<SkylightComponent>())
		{
			if (ImGui::Button("Sky Light"))
			{
				_selectionContext.AddComponent<SkylightComponent>();
				ImGui::CloseCurrentPopup();
			}
		}
		if (!_selectionContext.HasComponent<ScriptComponent>())
		{
			if (ImGui::Button("Script"))
			{
				_selectionContext.AddComponent<ScriptComponent>();
				ImGui::CloseCurrentPopup();
			}
		}
		if (!_selectionContext.HasComponent<SpriteRendererComponent>())
		{
			if (ImGui::Button("Sprite Renderer"))
			{
				_selectionContext.AddComponent<SpriteRendererComponent>();
				ImGui::CloseCurrentPopup();
			}
		}
		if (!_selectionContext.HasComponent<RigidBody2DComponent>())
		{
			if (ImGui::Button("Rigidbody 2D"))
			{
				_selectionContext.AddComponent<RigidBody2DComponent>();
				ImGui::CloseCurrentPopup();
			}
		}
		if (!_selectionContext.HasComponent<BoxCollider2DComponent>())
		{
			if (ImGui::Button("Box Collider 2D"))
			{
				_selectionContext.AddComponent<BoxCollider2DComponent>();
				ImGui::CloseCurrentPopup();
			}
		}
		if (!_selectionContext.HasComponent<CircleCollider2DComponent>())
		{
			if (ImGui::Button("Circle Collider 2D"))
			{
				_selectionContext.AddComponent<CircleCollider2DComponent>();
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::EndPopup();
	}

	DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
	{
		auto [translation, rotationQuat, scale] = Misc::GetTransformDecomposition(component);

		bool updateTransform = false;
		updateTransform |= DrawVec3Control("Translation", translation);
		glm::vec3 rotation = glm::degrees(glm::eulerAngles(rotationQuat));
		updateTransform |= DrawVec3Control("Rotation", rotation);
		updateTransform |= DrawVec3Control("Scale", scale, 1.0f);

		if (updateTransform)
		{
			component.Transform = glm::translate(glm::mat4(1.0f), translation) * toMat4(glm::quat(radians(rotation))) *
				glm::scale(glm::mat4(1.0f), scale);
		}
	});

	DrawComponent<MeshComponent>("Mesh", entity, [](MeshComponent& mc)
	{
		ImGui::Columns(3);
		ImGui::SetColumnWidth(0, 100);
		ImGui::SetColumnWidth(1, 300);
		ImGui::SetColumnWidth(2, 40);
		ImGui::Text("File Path");
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
		if (mc.Mesh)
			ImGui::InputText("##meshfilepath", const_cast<char*>(mc.Mesh->GetFilepath().string().c_str()), 256,
			                 ImGuiInputTextFlags_ReadOnly);
		else ImGui::InputText("##meshfilepath", static_cast<char*>("Null"), 256, ImGuiInputTextFlags_ReadOnly);
		ImGui::PopItemWidth();
		ImGui::NextColumn();
		if (ImGui::Button("...##openmesh"))
		{
			const auto filepath = FileIOManager::OpenFile(FileIOManager::Filter{"Mesh"});
			if (!filepath.empty())
			{
				mc.Mesh = Shared<Mesh>::Create(filepath);
			}
		}
		ImGui::Columns(1);
	});

	DrawComponent<CameraComponent>("Camera", entity, [](CameraComponent& cc)
	{
		// Projection Type
		const char* projTypeStrings[] = {"Perspective", "Orthographic"};
		const char* currentProj = projTypeStrings[static_cast<int>(cc.Camera->GetProjectionMode())];
		if (ImGui::BeginCombo("Projection", currentProj))
		{
			for (int type = 0; type < 2; type++)
			{
				bool is_selected = (currentProj == projTypeStrings[type]);
				if (ImGui::Selectable(projTypeStrings[type], is_selected))
				{
					currentProj = projTypeStrings[type];
					cc.Camera->SetProjectionMode(static_cast<SceneCamera::ProjectionMode>(type));
				}
				if (is_selected) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		Gui::BeginPropertyGrid();
		// Perspective parameters
		if (cc.Camera->GetProjectionMode() == SceneCamera::ProjectionMode::Perspective)
		{
			float verticalFOV = cc.Camera->GetPerspectiveVerticalFOV();
			if (Gui::Property("Vertical FOV", verticalFOV)) cc.Camera->SetPerspectiveVerticalFOV(verticalFOV);

			float nearClip = cc.Camera->GetPerspectiveNearClip();
			if (Gui::Property("Near Clip", nearClip)) cc.Camera->SetPerspectiveNearClip(nearClip);
			ImGui::SameLine();
			float farClip = cc.Camera->GetPerspectiveFarClip();
			if (Gui::Property("Far Clip", farClip)) cc.Camera->SetPerspectiveFarClip(farClip);
		}

			// Orthographic parameters
		else if (cc.Camera->GetProjectionMode() == SceneCamera::ProjectionMode::Orthographic)
		{
			float orthoSize = cc.Camera->GetOrthographicSize();
			if (Gui::Property("Size", orthoSize)) cc.Camera->SetOrthographicSize(orthoSize);

			float nearClip = cc.Camera->GetOrthographicNearClip();
			if (Gui::Property("Near Clip", nearClip)) cc.Camera->SetOrthographicNearClip(nearClip);
			ImGui::SameLine();
			float farClip = cc.Camera->GetOrthographicFarClip();
			if (Gui::Property("Far Clip", farClip)) cc.Camera->SetOrthographicFarClip(farClip);
		}

		Gui::EndPropertyGrid();
	});

	DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](SpriteRendererComponent& mc)
	{
	});

	DrawComponent<DirectionalLightComponent>("Directional Light", entity, [](DirectionalLightComponent& dlc)
	{
		Gui::BeginPropertyGrid();
		Gui::Property("Radiance", dlc.Radiance, Gui::PropertyFlag::Color);
		Gui::Property("Intensity", dlc.Intensity);
		Gui::Property("Cast Shadows", dlc.CastShadows);
		Gui::Property("Soft Shadows", dlc.SoftShadows);
		Gui::Property("Source Size", dlc.LightSize);
		Gui::EndPropertyGrid();
	});

	DrawComponent<SkylightComponent>("Sky Light", entity, [](SkylightComponent& slc)
	{
		ImGui::Columns(3);
		ImGui::SetColumnWidth(0, 100);
		ImGui::SetColumnWidth(1, 300);
		ImGui::SetColumnWidth(2, 40);
		ImGui::Text("File Path");
		ImGui::NextColumn();
		ImGui::PushItemWidth(-1);
		if (!slc.SceneEnvironment->GetFilepath().empty())
			ImGui::InputText("##envfilepath", const_cast<char*>(slc.SceneEnvironment->GetFilepath().string().c_str()),
			                 256, ImGuiInputTextFlags_ReadOnly);
		else ImGui::InputText("##envfilepath", static_cast<char*>("Empty"), 256, ImGuiInputTextFlags_ReadOnly);
		ImGui::PopItemWidth();
		ImGui::NextColumn();
		if (ImGui::Button("...##openenv"))
		{
			const auto filepath = FileIOManager::OpenFile(FileIOManager::Filter{".hdr", {".hdr"}});
			if (!filepath.empty())
			{
				slc.SceneEnvironment = SceneEnvironment::Load(filepath);
			}
		}
		ImGui::Columns(1);

		Gui::BeginPropertyGrid();
		Gui::Property("Intensity", slc.Intensity, 0.01f, 0.0f, 5.0f);
		Gui::EndPropertyGrid();
	});

	DrawComponent<ScriptComponent>("Script", entity, [=](ScriptComponent& sc) mutable
	{
		Gui::BeginPropertyGrid();
		std::string oldName = sc.ModuleName;
		if (Gui::Property("Module Name", sc.ModuleName, !ScriptEngine::ModuleExists(sc.ModuleName)))
			// TODO: no live edit
		{
			// Shutdown old script
			if (ScriptEngine::ModuleExists(oldName)) ScriptEngine::ShutdownScriptEntity(entity, oldName);

			if (ScriptEngine::ModuleExists(sc.ModuleName)) ScriptEngine::InitScriptEntity(entity);
		}

		// Public Fields
		if (ScriptEngine::ModuleExists(sc.ModuleName))
		{
			EntityInstanceData& entityInstanceData = ScriptEngine::GetEntityInstanceData(entity.GetSceneUUID(), id);
			auto& moduleFieldMap = entityInstanceData.ModuleFieldMap;
			if (moduleFieldMap.find(sc.ModuleName) != moduleFieldMap.end())
			{
				auto& publicFields = moduleFieldMap.at(sc.ModuleName);
				for (auto& [name, field] : publicFields)
				{
					bool isRuntime = dynamic_cast<RuntimeScene*>(_context.Raw()) && field.IsRuntimeAvailable();
					switch (field.Type)
					{
					case FieldType::Int:
					{
						int value = isRuntime ? field.GetRuntimeValue<int>() : field.GetStoredValue<int>();
						if (Gui::Property(field.Name.c_str(), value))
						{
							if (isRuntime) field.SetRuntimeValue(value);
							else field.SetStoredValue(value);
						}
						break;
					}
					case FieldType::Float:
					{
						float value = isRuntime ? field.GetRuntimeValue<float>() : field.GetStoredValue<float>();
						if (Gui::Property(field.Name.c_str(), value, 0.2f))
						{
							if (isRuntime) field.SetRuntimeValue(value);
							else field.SetStoredValue(value);
						}
						break;
					}
					case FieldType::Vec2:
					{
						glm::vec2 value = isRuntime
							                  ? field.GetRuntimeValue<glm::vec2>()
							                  : field.GetStoredValue<glm::vec2>();
						if (Gui::Property(field.Name.c_str(), value, 0.2f))
						{
							if (isRuntime) field.SetRuntimeValue(value);
							else field.SetStoredValue(value);
						}
						break;
					}
					case FieldType::Vec3:
					{
						glm::vec3 value = isRuntime
							                  ? field.GetRuntimeValue<glm::vec3>()
							                  : field.GetStoredValue<glm::vec3>();
						if (Gui::Property(field.Name.c_str(), value, 0.2f))
						{
							if (isRuntime) field.SetRuntimeValue(value);
							else field.SetStoredValue(value);
						}
						break;
					}
					case FieldType::Vec4:
					{
						glm::vec4 value = isRuntime
							                  ? field.GetRuntimeValue<glm::vec4>()
							                  : field.GetStoredValue<glm::vec4>();
						if (Gui::Property(field.Name.c_str(), value, 0.2f))
						{
							if (isRuntime) field.SetRuntimeValue(value);
							else field.SetStoredValue(value);
						}
						break;
					}
					}
				}
			}
		}

		Gui::EndPropertyGrid();
#if TODO
			if (ImGui::Button("Run Script"))
			{
				ScriptEngine::OnCreateEntity(entity);
			}
#endif
	});

	DrawComponent<RigidBody2DComponent>("Rigidbody 2D", entity, [](RigidBody2DComponent& rb2dc)
	{
		// Rigidbody2D Type
		const char* rb2dTypeStrings[] = {"Static", "Dynamic", "Kinematic"};
		const char* currentType = rb2dTypeStrings[static_cast<int>(rb2dc.BodyType)];
		if (ImGui::BeginCombo("Type", currentType))
		{
			for (int type = 0; type < 3; type++)
			{
				bool is_selected = (currentType == rb2dTypeStrings[type]);
				if (ImGui::Selectable(rb2dTypeStrings[type], is_selected))
				{
					currentType = rb2dTypeStrings[type];
					rb2dc.BodyType = static_cast<RigidBody2DComponent::Type>(type);
				}
				if (is_selected) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		if (rb2dc.BodyType == RigidBody2DComponent::Type::Dynamic)
		{
			Gui::BeginPropertyGrid();
			Gui::Property("Fixed Rotation", rb2dc.FixedRotation);
			Gui::EndPropertyGrid();
		}
	});

	DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](BoxCollider2DComponent& bc2dc)
	{
		Gui::BeginPropertyGrid();

		Gui::Property("Offset", bc2dc.Offset);
		Gui::Property("Size", bc2dc.Size);
		Gui::Property("Density", bc2dc.Density);
		Gui::Property("Friction", bc2dc.Friction);

		Gui::EndPropertyGrid();
	});

	DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](CircleCollider2DComponent& cc2dc)
	{
		Gui::BeginPropertyGrid();

		Gui::Property("Offset", cc2dc.Offset);
		Gui::Property("Radius", cc2dc.Radius);
		Gui::Property("Density", cc2dc.Density);
		Gui::Property("Friction", cc2dc.Friction);

		Gui::EndPropertyGrid();
	});
}

bool EntityComponentsPanel::DrawVec3Control(const String& label, Vector3f& value, float resetValue, float columnWidth)
{
	bool modified = false;

	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
	ImGui::PushFont(boldFont);
	if (ImGui::Button("X", buttonSize))
	{
		value.x = resetValue;
		modified = true;
	}

	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	modified |= ImGui::DragFloat("##X", &value.x, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Y", buttonSize))
	{
		value.y = resetValue;
		modified = true;
	}

	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	modified |= ImGui::DragFloat("##Y", &value.y, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Z", buttonSize))
	{
		value.z = resetValue;
		modified = true;
	}

	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	modified |= ImGui::DragFloat("##Z", &value.z, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();

	return modified;
}
}
