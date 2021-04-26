#include "SaffronPCH.h"

#include <yaml-cpp/yaml.h>

#include "Saffron/Math/SaffronMath.h"
#include "Saffron/Core/Misc.h"
#include "Saffron/Entity/Entity.h"
#include "Saffron/Entity/EntityComponents.h"
#include "Saffron/Serialize/SceneSerializer.h"
#include "Saffron/Serialize/SerializeHelpers.h"
#include "Saffron/Script/ScriptEngine.h"


///////////////////////////////////////////////////////////////////////////
/// Helper functions
///////////////////////////////////////////////////////////////////////////

#define BEGIN_YAML_COMPONENT_MAP(component) if(entity.HasComponent<component>()) {\
emitter << YAML::Key << #component; \
emitter << YAML::BeginMap

#define END_YAML_COMPONENT_MAP() emitter << YAML::EndMap; \
}

namespace Se
{
///////////////////////////////////////////////////////////////////////////
/// Scene Serializer
///////////////////////////////////////////////////////////////////////////

SceneSerializer::SceneSerializer(Scene& scene) :
	_scene(scene)
{
}

void SceneSerializer::Serialize(const Filepath& filepath) const
{
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Scene";
	out << YAML::Value << _scene.GetName();
	SerializeEnvironment(out);
	SerializeEditorCamera(out);
	out << YAML::Key << "Entities";
	out << YAML::Value << YAML::BeginSeq;
	_scene.GetEntityRegistry().each([&](auto entityID)
	{
		const Entity entity = {entityID, &_scene};
		if (!entity || !entity.HasComponent<IDComponent>()) return;

		SerializeEntity(out, entity);
	});
	out << YAML::EndSeq;
	out << YAML::EndMap;

	OStream fout(filepath);
	fout << out.c_str();
}

void SceneSerializer::SerializeRuntime(const Filepath& filepath) const
{
	// TODO: Implement
	Debug::Assert(false);;
}

bool SceneSerializer::Deserialize(const Filepath& filepath)
{
	IStream stream(filepath);
	OStringStream strStream;
	strStream << stream.rdbuf();

	YAML::Node data = YAML::Load(strStream.str());
	if (!data["Scene"]) return false;

	auto sceneName = data["Scene"].as<String>();
	Log::CoreInfo("Deserializing scene '{0}'", sceneName);
	_scene.SetName(sceneName);

	auto environment = data["Environment"];
	if (environment)
	{
		auto lightNode = environment["Light"];
		if (lightNode)
		{
			auto& light = _scene.GetLight();
			light.Direction = lightNode["Direction"].as<Vector3>();
			light.Radiance = lightNode["Radiance"].as<Vector3>();
			light.Multiplier = lightNode["Multiplier"].as<float>();
		}
	}

	auto editorCamera = data["EditorCamera"];
	if (editorCamera)
	{
		if (!_scene.GetEntity().HasComponent<EditorCameraComponent>())
		{
			_scene.GetEntity().AddComponent<EditorCameraComponent>();
		}
		auto& camera = _scene.GetEntity().GetComponent<EditorCameraComponent>().Camera;

		auto projectionMatrix = editorCamera["ProjectionMatrix"];
		if (projectionMatrix)
		{
			camera->SetProjectionMatrix(projectionMatrix.as<Matrix4>());
		}

		auto position = editorCamera["Position"];
		if (position)
		{
			camera->SetPosition(position.as<Vector3>());
		}

		auto rotation = editorCamera["Rotation"];
		if (rotation)
		{
			camera->SetRotation(rotation.as<Vector3>());
		}
	}


	auto entities = data["Entities"];
	if (entities)
	{
		for (auto entity : entities)
		{
			auto uuid = entity["Entity"].as<ulong>();

			String name;
			auto tagComponent = entity["TagComponent"];
			if (tagComponent) name = tagComponent["Tag"].as<String>();

			Entity deserializedEntity = _scene.CreateEntity(uuid, name);

			auto transformComponent = entity["TransformComponent"];
			if (transformComponent)
			{
				// Entities always have transforms
				auto& transform = deserializedEntity.GetComponent<TransformComponent>().Transform;
				auto translation = transformComponent["Position"].as<Vector3>();
				auto rotation = transformComponent["Rotation"].as<Quaternion>();
				auto scale = transformComponent["Scale"].as<Vector3>();

				transform = Math::ComposeMatrix(translation, rotation, scale);
			}

			auto scriptComponent = entity["ScriptComponent"];
			if (scriptComponent)
			{
				auto moduleName = scriptComponent["ModuleName"].as<String>();
				deserializedEntity.AddComponent<ScriptComponent>(moduleName);

				if (ScriptEngine::ModuleExists(moduleName))
				{
					auto storedFields = scriptComponent["StoredFields"];
					if (storedFields)
					{
						for (auto field : storedFields)
						{
							// TODO: Look over overwritten variables
							auto name = field["Name"].as<String>();
							FieldType type = static_cast<FieldType>(field["Type"].as<uint>());
							EntityInstance& instance = ScriptEngine::GetEntityInstance(_scene.GetUUID(), uuid);
							auto& moduleFieldMap = instance.ModuleFieldMap;
							auto& publicFields = moduleFieldMap[moduleName];
							if (publicFields.find(name) == publicFields.end())
							{
								PublicField pf = {name, type};
								publicFields.emplace(name, Move(pf));
							}
							auto dataNode = field["Data"];
							switch (type)
							{
							case FieldType::Float:
							{
								publicFields.at(name).SetStoredValue(dataNode.as<float>());
								break;
							}
							case FieldType::Int:
							{
								publicFields.at(name).SetStoredValue(dataNode.as<int>());
								break;
							}
							case FieldType::Uint:
							{
								publicFields.at(name).SetStoredValue(dataNode.as<uint32_t>());
								break;
							}
							case FieldType::String:
							{
								Debug::Break("Unimplemented");
								break;
							}
							case FieldType::Vec2:
							{
								publicFields.at(name).SetStoredValue(dataNode.as<Vector2>());
								break;
							}
							case FieldType::Vec3:
							{
								publicFields.at(name).SetStoredValue(dataNode.as<Vector3>());
								break;
							}
							case FieldType::Vec4:
							{
								publicFields.at(name).SetStoredValue(dataNode.as<Vector4>());
								break;
							}
							default: break;
							}
						}
					}
				}
			}

			auto meshComponent = entity["MeshComponent"];
			if (meshComponent)
			{
				auto& component = deserializedEntity.AddComponent<MeshComponent>();
				auto meshPath = meshComponent["AssetPath"].as<String>();
				auto translation = meshComponent["Position"].as<Vector3>();
				auto rotation = meshComponent["Rotation"].as<Quaternion>();
				auto scale = meshComponent["Scale"].as<Vector3>();
				component.Mesh = Mesh::Create(meshPath);
				component.Mesh->SetLocalTransform(Math::ComposeMatrix(translation, rotation, scale));
			}

			auto cameraComponent = entity["CameraComponent"];
			if (cameraComponent)
			{
				auto& component = deserializedEntity.AddComponent<CameraComponent>();
				auto translation = cameraComponent["Position"].as<Vector3>();
				auto rotation = cameraComponent["Rotation"].as<Quaternion>();
				auto scale = cameraComponent["Scale"].as<Vector3>();
				component.Camera = Shared<SceneCamera>::Create();
				component.Primary = cameraComponent["Primary"].as<bool>();
				component.DrawMesh = cameraComponent["DrawMesh"].as<bool>();
				component.DrawFrustum = cameraComponent["DrawFrustum"].as<bool>();
				component.CameraMesh->SetLocalTransform(Math::ComposeMatrix(translation, rotation, scale));
			}

			auto directionalLightComponent = entity["DirectionalLightComponent"];
			if (directionalLightComponent)
			{
				auto& component = deserializedEntity.AddComponent<DirectionalLightComponent>();
				component.Radiance = directionalLightComponent["Radiance"].as<Vector3>();
				component.CastShadows = directionalLightComponent["CastShadows"].as<bool>();
				component.SoftShadows = directionalLightComponent["SoftShadows"].as<bool>();
				component.LightSize = directionalLightComponent["LightSize"].as<float>();
			}

			auto skyLightComponent = entity["SkylightComponent"];
			if (skyLightComponent)
			{
				Filepath environmentFilepath = skyLightComponent["EnvironmentAssetPath"].as<String>();
				auto& component = deserializedEntity.AddComponent<SkylightComponent>(
					SceneEnvironment::Create(environmentFilepath));
				component.Intensity = skyLightComponent["Intensity"].as<float>();
				component.Angle = skyLightComponent["Angle"].as<float>();
			}

			auto spriteRendererComponent = entity["SpriteRendererComponent"];
			if (spriteRendererComponent)
			{
				auto& component = deserializedEntity.AddComponent<SpriteRendererComponent>();
				component.Color = spriteRendererComponent["Color"].as<Vector4>();
				component.TilingFactor = spriteRendererComponent["TilingFactor"].as<float>();
			}

			auto rigidBody2DComponent = entity["RigidBody2DComponent"];
			if (rigidBody2DComponent)
			{
				auto& component = deserializedEntity.AddComponent<RigidBody2DComponent>();
				component.BodyType = static_cast<RigidBody2DComponent::Type>(rigidBody2DComponent["BodyType"].as<int>()
				);
				component.FixedRotation = rigidBody2DComponent["FixedRotation"]
					                          ? rigidBody2DComponent["FixedRotation"].as<bool>()
					                          : false;
			}

			auto boxCollider2DComponent = entity["BoxCollider2DComponent"];
			if (boxCollider2DComponent)
			{
				auto& component = deserializedEntity.AddComponent<BoxCollider2DComponent>();
				component.Offset = boxCollider2DComponent["Offset"].as<Vector2>();
				component.Size = boxCollider2DComponent["Size"].as<Vector2>();
				component.Density = boxCollider2DComponent["Density"]
					                    ? boxCollider2DComponent["Density"].as<float>()
					                    : 1.0f;
				component.Friction = boxCollider2DComponent["Friction"]
					                     ? boxCollider2DComponent["Friction"].as<float>()
					                     : 1.0f;
				component.DrawBounding = boxCollider2DComponent["DrawBounding"]
					                         ? boxCollider2DComponent["DrawBounding"].as<bool>()
					                         : 1.0f;
			}

			auto circleCollider2DComponent = entity["CircleCollider2DComponent"];
			if (circleCollider2DComponent)
			{
				auto& component = deserializedEntity.AddComponent<CircleCollider2DComponent>();
				component.Offset = circleCollider2DComponent["Offset"].as<Vector2>();
				component.Radius = circleCollider2DComponent["Radius"].as<float>();
				component.Density = circleCollider2DComponent["Density"]
					                    ? circleCollider2DComponent["Density"].as<float>()
					                    : 1.0f;
				component.Friction = circleCollider2DComponent["Friction"]
					                     ? circleCollider2DComponent["Friction"].as<float>()
					                     : 1.0f;
				component.DrawBounding = circleCollider2DComponent["DrawBounding"]
					                         ? circleCollider2DComponent["DrawBounding"].as<bool>()
					                         : 1.0f;
			}

			auto rigidBody3DComponent = entity["RigidBody3DComponent"];
			if (rigidBody3DComponent)
			{
				auto& component = deserializedEntity.AddComponent<RigidBody3DComponent>();
				component.BodyType = static_cast<RigidBody3DComponent::Type>(rigidBody3DComponent["BodyType"].as<int>()
				);
			}

			auto boxCollider3DComponent = entity["BoxCollider3DComponent"];
			if (boxCollider3DComponent)
			{
				auto& component = deserializedEntity.AddComponent<BoxCollider3DComponent>();
				component.Offset = boxCollider3DComponent["Offset"].as<Vector3>();
				component.Size = boxCollider3DComponent["Size"].as<Vector3>();
				component.Density = boxCollider3DComponent["Density"]
					                    ? boxCollider3DComponent["Density"].as<float>()
					                    : 1.0f;
				component.Friction = boxCollider3DComponent["Friction"]
					                     ? boxCollider3DComponent["Friction"].as<float>()
					                     : 1.0f;
				component.DrawBounding = boxCollider3DComponent["DrawBounding"]
					                         ? boxCollider3DComponent["DrawBounding"].as<bool>()
					                         : 1.0f;
			}

			auto sphereCollider3DComponent = entity["SphereCollider3DComponent"];
			if (sphereCollider3DComponent)
			{
				auto& component = deserializedEntity.AddComponent<SphereCollider3DComponent>();
				component.Offset = sphereCollider3DComponent["Offset"].as<Vector3>();
				component.Radius = sphereCollider3DComponent["Radius"].as<float>();
				component.Density = sphereCollider3DComponent["Density"]
					                    ? sphereCollider3DComponent["Density"].as<float>()
					                    : 1.0f;
				component.Friction = sphereCollider3DComponent["Friction"]
					                     ? sphereCollider3DComponent["Friction"].as<float>()
					                     : 1.0f;
				component.DrawBounding = sphereCollider3DComponent["DrawBounding"]
					                         ? sphereCollider3DComponent["DrawBounding"].as<bool>()
					                         : 1.0f;
			}
		}
	}
	return true;
}

bool SceneSerializer::DeserializeRuntime(const Filepath& filepath)
{
	// Not implemented
	Debug::Assert(false);;
	return false;
}

void SceneSerializer::SerializeEntity(YAML::Emitter& emitter, Entity entity) const
{
	UUID uuid = entity.GetComponent<IDComponent>().ID;
	emitter << YAML::BeginMap; // Entity
	emitter << YAML::Key << "Entity" << YAML::Value << uuid;


	BEGIN_YAML_COMPONENT_MAP(TagComponent);
		auto& tag = entity.GetComponent<TagComponent>().Tag;
		emitter << YAML::Key << "Tag" << YAML::Value << tag;
		END_YAML_COMPONENT_MAP();

	BEGIN_YAML_COMPONENT_MAP(TransformComponent);
		auto& transform = entity.GetComponent<TransformComponent>().Transform;
		const auto [translation, rotation, scale] = Misc::GetTransformDecomposition(transform);
		emitter << YAML::Key << "Position" << YAML::Value << translation;
		emitter << YAML::Key << "Rotation" << YAML::Value << rotation;
		emitter << YAML::Key << "Scale" << YAML::Value << scale;
		END_YAML_COMPONENT_MAP();

	BEGIN_YAML_COMPONENT_MAP(ScriptComponent);
		auto& moduleName = entity.GetComponent<ScriptComponent>().ModuleName;
		emitter << YAML::Key << "ModuleName" << YAML::Value << moduleName;

		EntityInstance& instance = ScriptEngine::GetEntityInstance(entity.GetSceneUUID(), uuid);
		const auto& moduleFieldMap = instance.ModuleFieldMap;
		if (moduleFieldMap.find(moduleName) != moduleFieldMap.end())
		{
			const auto& fields = moduleFieldMap.at(moduleName);
			emitter << YAML::Key << "StoredFields" << YAML::Value;
			emitter << YAML::BeginSeq;
			for (const auto& [name, field] : fields)
			{
				emitter << YAML::BeginMap; // Field
				emitter << YAML::Key << "Name" << YAML::Value << name;
				emitter << YAML::Key << "Type" << YAML::Value << static_cast<uint32_t>(field.Type);
				emitter << YAML::Key << "Data" << YAML::Value;

				switch (field.Type)
				{
				case FieldType::Int: emitter << field.GetStoredValue<int>();
					break;
				case FieldType::Uint: emitter << field.GetStoredValue<uint32_t>();
					break;
				case FieldType::Float: emitter << field.GetStoredValue<float>();
					break;
				case FieldType::Vec2: emitter << field.GetStoredValue<Vector2>();
					break;
				case FieldType::Vec3: emitter << field.GetStoredValue<Vector3>();
					break;
				case FieldType::Vec4: emitter << field.GetStoredValue<Vector4>();
					break;
				default: emitter << "Invalid value";
				}
				emitter << YAML::EndMap; // Field
			}
			emitter << YAML::EndSeq;
		}
		END_YAML_COMPONENT_MAP();

	BEGIN_YAML_COMPONENT_MAP(MeshComponent);
		auto meshComponent = entity.GetComponent<MeshComponent>();
		emitter << YAML::Key << "AssetPath" << YAML::Value << meshComponent.Mesh->GetFilepath().filename().string();
		const auto [translation, rotation, scale] = Misc::GetTransformDecomposition(
			meshComponent.Mesh->GetLocalTransform());
		emitter << YAML::Key << "Position" << YAML::Value << translation;
		emitter << YAML::Key << "Rotation" << YAML::Value << rotation;
		emitter << YAML::Key << "Scale" << YAML::Value << scale;
		END_YAML_COMPONENT_MAP();

	BEGIN_YAML_COMPONENT_MAP(CameraComponent);
		auto& cameraComponent = entity.GetComponent<CameraComponent>();
		emitter << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
		emitter << YAML::Key << "DrawMesh" << YAML::Value << cameraComponent.DrawMesh;
		emitter << YAML::Key << "DrawFrustum" << YAML::Value << cameraComponent.DrawFrustum;
		const auto [translation, rotation, scale] = Misc::GetTransformDecomposition(
			cameraComponent.CameraMesh->GetLocalTransform());
		emitter << YAML::Key << "Position" << YAML::Value << translation;
		emitter << YAML::Key << "Rotation" << YAML::Value << rotation;
		emitter << YAML::Key << "Scale" << YAML::Value << scale;
		END_YAML_COMPONENT_MAP();

	BEGIN_YAML_COMPONENT_MAP(DirectionalLightComponent);
		auto& directionalLightComponent = entity.GetComponent<DirectionalLightComponent>();
		emitter << YAML::Key << "Radiance" << YAML::Value << directionalLightComponent.Radiance;
		emitter << YAML::Key << "CastShadows" << YAML::Value << directionalLightComponent.CastShadows;
		emitter << YAML::Key << "SoftShadows" << YAML::Value << directionalLightComponent.SoftShadows;
		emitter << YAML::Key << "LightSize" << YAML::Value << directionalLightComponent.LightSize;
		END_YAML_COMPONENT_MAP();

	BEGIN_YAML_COMPONENT_MAP(SkylightComponent);
		auto& skyLightComponent = entity.GetComponent<SkylightComponent>();
		emitter << YAML::Key << "EnvironmentAssetPath" << YAML::Value << skyLightComponent.SceneEnvironment->
			GetFilepath().string();
		emitter << YAML::Key << "Intensity" << YAML::Value << skyLightComponent.Intensity;
		emitter << YAML::Key << "Angle" << YAML::Value << skyLightComponent.Angle;
		END_YAML_COMPONENT_MAP();

	BEGIN_YAML_COMPONENT_MAP(SpriteRendererComponent);
		auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();
		emitter << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.Color;
		if (spriteRendererComponent.Texture)
			emitter << YAML::Key << "TextureAssetPath" << YAML::Value << "path/to/asset";
		emitter << YAML::Key << "TilingFactor" << YAML::Value << spriteRendererComponent.TilingFactor;
		END_YAML_COMPONENT_MAP();

	BEGIN_YAML_COMPONENT_MAP(RigidBody2DComponent);
		auto& rigidbody2DComponent = entity.GetComponent<RigidBody2DComponent>();
		emitter << YAML::Key << "BodyType" << YAML::Value << static_cast<int>(rigidbody2DComponent.BodyType);
		emitter << YAML::Key << "FixedRotation" << YAML::Value << rigidbody2DComponent.FixedRotation;
		END_YAML_COMPONENT_MAP();

	BEGIN_YAML_COMPONENT_MAP(BoxCollider2DComponent);
		auto& boxCollider2DComponent = entity.GetComponent<BoxCollider2DComponent>();
		emitter << YAML::Key << "Offset" << YAML::Value << boxCollider2DComponent.Offset;
		emitter << YAML::Key << "Size" << YAML::Value << boxCollider2DComponent.Size;
		emitter << YAML::Key << "Density" << YAML::Value << boxCollider2DComponent.Density;
		emitter << YAML::Key << "Friction" << YAML::Value << boxCollider2DComponent.Friction;
		emitter << YAML::Key << "DrawBounding" << YAML::Value << boxCollider2DComponent.DrawBounding;
		END_YAML_COMPONENT_MAP();

	BEGIN_YAML_COMPONENT_MAP(CircleCollider2DComponent);
		auto& circleCollider2DComponent = entity.GetComponent<CircleCollider2DComponent>();
		emitter << YAML::Key << "Offset" << YAML::Value << circleCollider2DComponent.Offset;
		emitter << YAML::Key << "Radius" << YAML::Value << circleCollider2DComponent.Radius;
		emitter << YAML::Key << "Density" << YAML::Value << circleCollider2DComponent.Density;
		emitter << YAML::Key << "Friction" << YAML::Value << circleCollider2DComponent.Friction;
		emitter << YAML::Key << "DrawBounding" << YAML::Value << circleCollider2DComponent.DrawBounding;
		END_YAML_COMPONENT_MAP();

	BEGIN_YAML_COMPONENT_MAP(RigidBody3DComponent);
		auto& rigidbody3DComponent = entity.GetComponent<RigidBody3DComponent>();
		emitter << YAML::Key << "BodyType" << YAML::Value << static_cast<int>(rigidbody3DComponent.BodyType);
		END_YAML_COMPONENT_MAP();

	BEGIN_YAML_COMPONENT_MAP(BoxCollider3DComponent);
		auto& boxCollider3DComponent = entity.GetComponent<BoxCollider3DComponent>();
		emitter << YAML::Key << "Offset" << YAML::Value << boxCollider3DComponent.Offset;
		emitter << YAML::Key << "Size" << YAML::Value << boxCollider3DComponent.Size;
		emitter << YAML::Key << "Density" << YAML::Value << boxCollider3DComponent.Density;
		emitter << YAML::Key << "Friction" << YAML::Value << boxCollider3DComponent.Friction;
		emitter << YAML::Key << "DrawBounding" << YAML::Value << boxCollider3DComponent.DrawBounding;
		END_YAML_COMPONENT_MAP();

	BEGIN_YAML_COMPONENT_MAP(SphereCollider3DComponent);
		auto& sphereCollider3DComponent = entity.GetComponent<SphereCollider3DComponent>();
		emitter << YAML::Key << "Offset" << YAML::Value << sphereCollider3DComponent.Offset;
		emitter << YAML::Key << "Radius" << YAML::Value << sphereCollider3DComponent.Radius;
		emitter << YAML::Key << "Density" << YAML::Value << sphereCollider3DComponent.Density;
		emitter << YAML::Key << "Friction" << YAML::Value << sphereCollider3DComponent.Friction;
		emitter << YAML::Key << "DrawBounding" << YAML::Value << sphereCollider3DComponent.DrawBounding;
		END_YAML_COMPONENT_MAP();

	emitter << YAML::EndMap; // Entity
}

void SceneSerializer::SerializeEnvironment(YAML::Emitter& emitter) const
{
	emitter << YAML::Key << "Environment";
	emitter << YAML::Value;
	emitter << YAML::BeginMap; // Environment
	emitter << YAML::Key << "AssetPath" << YAML::Value << _scene.GetSceneEnvironment()->GetFilepath().string();
	const auto& light = _scene.GetLight();
	emitter << YAML::Key << "Light" << YAML::Value;
	emitter << YAML::BeginMap; // Light
	emitter << YAML::Key << "Direction" << YAML::Value << light.Direction;
	emitter << YAML::Key << "Radiance" << YAML::Value << light.Radiance;
	emitter << YAML::Key << "Multiplier" << YAML::Value << light.Multiplier;
	emitter << YAML::EndMap; // Light
	emitter << YAML::EndMap; // Environment
}

void SceneSerializer::SerializeEditorCamera(YAML::Emitter& emitter) const
{
	if (!_scene.GetEntity().HasComponent<EditorCameraComponent>())
	{
		return;
	}

	const auto& camera = _scene.GetEntity().GetComponent<EditorCameraComponent>().Camera;

	emitter << YAML::Key << "EditorCamera" << YAML::Value;
	emitter << YAML::BeginMap;
	emitter << YAML::Key << "Position" << YAML::Value << camera->GetPosition();
	emitter << YAML::Key << "Rotation" << YAML::Value << Vector3(camera->GetPitch(), camera->GetYaw(),
	                                                             camera->GetRoll());
	emitter << YAML::Key << "PerspectiveMatrix" << YAML::Value << camera->GetProjectionMatrix();
	emitter << YAML::EndMap;
}
}
