#include "SaffronPCH.h"

#include <yaml-cpp/yaml.h>

#include "Saffron/Core/Math/SaffronMath.h"
#include "Saffron/Core/Misc.h"
#include "Saffron/Entity/Entity.h"
#include "Saffron/Entity/EntityComponents.h"
#include "Saffron/Serialize/SceneSerializer.h"
#include "Saffron/Script/ScriptEngine.h"

///////////////////////////////////////////////////////////////////////////
/// YAML - Template Specialization
///////////////////////////////////////////////////////////////////////////

namespace YAML {

template<>
struct convert<Se::Vector2f>
{
	static Node encode(const Se::Vector2f &rhs)
	{
		Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		return node;
	}

	static bool decode(const Node &node, Se::Vector2f &rhs)
	{
		if ( !node.IsSequence() || node.size() != 2 )
			return false;

		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();
		return true;
	}
};

template<>
struct convert<Se::Vector3f>
{
	static Node encode(const Se::Vector3f &rhs)
	{
		Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		node.push_back(rhs.z);
		return node;
	}

	static bool decode(const Node &node, Se::Vector3f &rhs)
	{
		if ( !node.IsSequence() || node.size() != 3 )
			return false;

		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();
		rhs.z = node[2].as<float>();
		return true;
	}
};

template<>
struct convert<Se::Vector4f>
{
	static Node encode(const Se::Vector4f &rhs)
	{
		Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		node.push_back(rhs.z);
		node.push_back(rhs.w);
		return node;
	}

	static bool decode(const Node &node, Se::Vector4f &rhs)
	{
		if ( !node.IsSequence() || node.size() != 4 )
			return false;

		rhs.x = node[0].as<float>();
		rhs.y = node[1].as<float>();
		rhs.z = node[2].as<float>();
		rhs.w = node[3].as<float>();
		return true;
	}
};

template<>
struct convert<Se::Quaternion>
{
	static Node encode(const glm::quat &rhs)
	{
		Node node;
		node.push_back(rhs.w);
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		node.push_back(rhs.z);
		return node;
	}

	static bool decode(const Node &node, Se::Quaternion &rhs)
	{
		if ( !node.IsSequence() || node.size() != 4 )
			return false;

		rhs.w = node[0].as<float>();
		rhs.x = node[1].as<float>();
		rhs.y = node[2].as<float>();
		rhs.z = node[3].as<float>();
		return true;
	}
};
}


namespace Se
{

///////////////////////////////////////////////////////////////////////////
/// YAML - Operator overloading
///////////////////////////////////////////////////////////////////////////

YAML::Emitter &operator<<(YAML::Emitter &out, const Vector2f &v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
	return out;
}

YAML::Emitter &operator<<(YAML::Emitter &out, const Vector3f &v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
	return out;
}

YAML::Emitter &operator<<(YAML::Emitter &out, const Vector4f &v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
	return out;
}

YAML::Emitter &operator<<(YAML::Emitter &out, const Quaternion &v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.w << v.x << v.y << v.z << YAML::EndSeq;
	return out;
}


///////////////////////////////////////////////////////////////////////////
/// Helper functions
///////////////////////////////////////////////////////////////////////////

#define BEGIN_YAML_COMPONENT_MAP(component) if(entity.HasComponent<component>()) {\
out << YAML::Key << #component; \
out << YAML::BeginMap

#define END_YAML_COMPONENT_MAP() out << YAML::EndMap; \
}

static void SerializeEntity(YAML::Emitter &out, Entity entity)
{
	UUID uuid = entity.GetComponent<IDComponent>().ID;
	out << YAML::BeginMap; // Entity
	out << YAML::Key << "Entity";
	out << YAML::Value << uuid;

	BEGIN_YAML_COMPONENT_MAP(TagComponent);
	auto &tag = entity.GetComponent<TagComponent>().Tag;
	out << YAML::Key << "Tag" << YAML::Value << tag;
	END_YAML_COMPONENT_MAP();

	BEGIN_YAML_COMPONENT_MAP(TransformComponent);
	auto &transform = entity.GetComponent<TransformComponent>().Transform;
	const auto decomposition = Misc::GetTransformDecomposition(transform);
	out << YAML::Key << "Position" << YAML::Value << decomposition.Translation;
	out << YAML::Key << "Rotation" << YAML::Value << decomposition.Rotation;
	out << YAML::Key << "Scale" << YAML::Value << decomposition.Scale;
	END_YAML_COMPONENT_MAP();

	BEGIN_YAML_COMPONENT_MAP(ScriptComponent);
	auto &moduleName = entity.GetComponent<ScriptComponent>().ModuleName;
	out << YAML::Key << "ModuleName" << YAML::Value << moduleName;

	EntityInstanceData &data = ScriptEngine::GetEntityInstanceData(entity.GetSceneUUID(), uuid);
	const auto &moduleFieldMap = data.ModuleFieldMap;
	if ( moduleFieldMap.find(moduleName) != moduleFieldMap.end() )
	{
		const auto &fields = moduleFieldMap.at(moduleName);
		out << YAML::Key << "StoredFields" << YAML::Value;
		out << YAML::BeginSeq;
		for ( const auto &[name, field] : fields )
		{
			out << YAML::BeginMap; // Field
			out << YAML::Key << "Name" << YAML::Value << name;
			out << YAML::Key << "Type" << YAML::Value << static_cast<uint32_t>(field.Type);
			out << YAML::Key << "Data" << YAML::Value;

			switch ( field.Type )
			{
			case FieldType::Int:
				out << field.GetStoredValue<int>();
				break;
			case FieldType::UnsignedInt:
				out << field.GetStoredValue<uint32_t>();
				break;
			case FieldType::Float:
				out << field.GetStoredValue<float>();
				break;
			case FieldType::Vec2:
				out << field.GetStoredValue<Vector2f>();
				break;
			case FieldType::Vec3:
				out << field.GetStoredValue<Vector3f>();
				break;
			case FieldType::Vec4:
				out << field.GetStoredValue<Vector4f>();
				break;
			default:
				out << "Invalid value";
			}
			out << YAML::EndMap; // Field
		}
		out << YAML::EndSeq;
	}
	END_YAML_COMPONENT_MAP();

	BEGIN_YAML_COMPONENT_MAP(MeshComponent);
	auto mesh = entity.GetComponent<MeshComponent>().Mesh;
	out << YAML::Key << "AssetPath" << YAML::Value << mesh->GetFilepath();
	END_YAML_COMPONENT_MAP();

	BEGIN_YAML_COMPONENT_MAP(CameraComponent);
	auto &cameraComponent = entity.GetComponent<CameraComponent>();
	out << YAML::Key << "Camera" << YAML::Value << "some camera data...";
	out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
	END_YAML_COMPONENT_MAP();

	BEGIN_YAML_COMPONENT_MAP(SpriteRendererComponent);
	auto &spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();
	out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.Color;
	if ( spriteRendererComponent.Texture )
		out << YAML::Key << "TextureAssetPath" << YAML::Value << "path/to/asset";
	out << YAML::Key << "TilingFactor" << YAML::Value << spriteRendererComponent.TilingFactor;
	END_YAML_COMPONENT_MAP();

	BEGIN_YAML_COMPONENT_MAP(RigidBody2DComponent);
	auto &rigidbody2DComponent = entity.GetComponent<RigidBody2DComponent>();
	out << YAML::Key << "BodyType" << YAML::Value << static_cast<int>(rigidbody2DComponent.BodyType);
	out << YAML::Key << "FixedRotation" << YAML::Value << rigidbody2DComponent.FixedRotation;
	END_YAML_COMPONENT_MAP();

	BEGIN_YAML_COMPONENT_MAP(BoxCollider2DComponent);
	auto &boxCollider2DComponent = entity.GetComponent<BoxCollider2DComponent>();
	out << YAML::Key << "Offset" << YAML::Value << boxCollider2DComponent.Offset;
	out << YAML::Key << "Size" << YAML::Value << boxCollider2DComponent.Size;
	out << YAML::Key << "Density" << YAML::Value << boxCollider2DComponent.Density;
	out << YAML::Key << "Friction" << YAML::Value << boxCollider2DComponent.Friction;
	END_YAML_COMPONENT_MAP();

	BEGIN_YAML_COMPONENT_MAP(CircleCollider2DComponent);
	auto &circleCollider2DComponent = entity.GetComponent<CircleCollider2DComponent>();
	out << YAML::Key << "Offset" << YAML::Value << circleCollider2DComponent.Offset;
	out << YAML::Key << "Radius" << YAML::Value << circleCollider2DComponent.Radius;
	out << YAML::Key << "Density" << YAML::Value << circleCollider2DComponent.Density;
	out << YAML::Key << "Friction" << YAML::Value << circleCollider2DComponent.Friction;
	END_YAML_COMPONENT_MAP();

	BEGIN_YAML_COMPONENT_MAP(RigidBody3DComponent);
	auto &rigidbody3DComponent = entity.GetComponent<RigidBody3DComponent>();
	out << YAML::Key << "BodyType" << YAML::Value << static_cast<int>(rigidbody3DComponent.BodyType);
	END_YAML_COMPONENT_MAP();

	BEGIN_YAML_COMPONENT_MAP(BoxCollider3DComponent);
	auto &boxCollider3DComponent = entity.GetComponent<BoxCollider3DComponent>();
	out << YAML::Key << "Offset" << YAML::Value << boxCollider3DComponent.Offset;
	out << YAML::Key << "Size" << YAML::Value << boxCollider3DComponent.Size;
	out << YAML::Key << "Density" << YAML::Value << boxCollider3DComponent.Density;
	out << YAML::Key << "Friction" << YAML::Value << boxCollider3DComponent.Friction;
	END_YAML_COMPONENT_MAP();

	BEGIN_YAML_COMPONENT_MAP(SphereCollider3DComponent);
	auto &sphereCollider3DComponent = entity.GetComponent<SphereCollider3DComponent>();
	out << YAML::Key << "Offset" << YAML::Value << sphereCollider3DComponent.Offset;
	out << YAML::Key << "Radius" << YAML::Value << sphereCollider3DComponent.Radius;
	out << YAML::Key << "Density" << YAML::Value << sphereCollider3DComponent.Density;
	out << YAML::Key << "Friction" << YAML::Value << sphereCollider3DComponent.Friction;
	END_YAML_COMPONENT_MAP();

	out << YAML::EndMap; // Entity
}

static void SerializeEnvironment(YAML::Emitter &out, const Scene &scene)
{
	out << YAML::Key << "Environment";
	out << YAML::Value;
	out << YAML::BeginMap; // Environment
	out << YAML::Key << "AssetPath" << YAML::Value << scene.GetEnvironment().FilePath;
	const auto &light = scene.GetLight();
	out << YAML::Key << "Light" << YAML::Value;
	out << YAML::BeginMap; // Light
	out << YAML::Key << "Direction" << YAML::Value << light.Direction;
	out << YAML::Key << "Radiance" << YAML::Value << light.Radiance;
	out << YAML::Key << "Multiplier" << YAML::Value << light.Multiplier;
	out << YAML::EndMap; // Light
	out << YAML::EndMap; // Environment
}


///////////////////////////////////////////////////////////////////////////
/// Scene Serializer
///////////////////////////////////////////////////////////////////////////

SceneSerializer::SceneSerializer(Scene &scene)
	: m_Scene(scene)
{
}

void SceneSerializer::Serialize(const Filepath &filepath)
{
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Scene";
	out << YAML::Value << m_Scene.GetName();
	SerializeEnvironment(out, m_Scene);
	out << YAML::Key << "Entities";
	out << YAML::Value << YAML::BeginSeq;
	m_Scene.GetEntityRegistry().each([&](auto entityID)
									 {
										 Entity entity = { entityID, &m_Scene };
										 if ( !entity || !entity.HasComponent<IDComponent>() )
											 return;

										 SerializeEntity(out, entity);

									 });
	out << YAML::EndSeq;
	out << YAML::EndMap;

	OutputStream fout(filepath);
	fout << out.c_str();
}

void SceneSerializer::SerializeRuntime(const Filepath &filepath)
{
	// TODO: Implement
	SE_CORE_ASSERT(false);
}

bool SceneSerializer::Deserialize(const Filepath &filepath)
{
	InputStream stream(filepath);
	StringStream strStream;
	strStream << stream.rdbuf();

	YAML::Node data = YAML::Load(strStream.str());
	if ( !data["Scene"] )
		return false;

	auto sceneName = data["Scene"].as<String>();
	SE_CORE_INFO("Deserializing scene '{0}'", sceneName);
	m_Scene.SetName(sceneName);

	auto environment = data["Environment"];
	if ( environment )
	{
		auto envPath = environment["AssetPath"].as<String>();
		m_Scene.SetEnvironment(Scene::Environment::Load(envPath));

		auto lightNode = environment["Light"];
		if ( lightNode )
		{
			auto &light = m_Scene.GetLight();
			light.Direction = lightNode["Direction"].as<Vector3f>();
			light.Radiance = lightNode["Radiance"].as<Vector3f>();
			light.Multiplier = lightNode["Multiplier"].as<float>();
		}
	}

	auto entities = data["Entities"];
	if ( entities )
	{
		for ( auto entity : entities )
		{
			auto uuid = entity["Entity"].as<uint64_t>();

			String name;
			auto tagComponent = entity["TagComponent"];
			if ( tagComponent )
				name = tagComponent["Tag"].as<String>();

			SE_CORE_INFO("Deserialized entity with ID = {0}, name = {1}", uuid, name);

			Entity deserializedEntity = m_Scene.CreateEntity(uuid, name);

			auto transformComponent = entity["TransformComponent"];
			if ( transformComponent )
			{
				// Entities always have transforms
				auto &transform = deserializedEntity.GetComponent<TransformComponent>().Transform;
				auto translation = transformComponent["Position"].as<Vector3f>();
				auto rotation = transformComponent["Rotation"].as<glm::quat>();
				auto scale = transformComponent["Scale"].as<Vector3f>();

				transform = glm::translate(Matrix4f(1.0f), translation) *
					glm::toMat4(rotation) * glm::scale(Matrix4f(1.0f), scale);

				SE_CORE_INFO("  Entity Transform:");
				SE_CORE_INFO("    Translation: {0}, {1}, {2}", translation.x, translation.y, translation.z);
				SE_CORE_INFO("    Rotation: {0}, {1}, {2}, {3}", rotation.w, rotation.x, rotation.y, rotation.z);
				SE_CORE_INFO("    Scale: {0}, {1}, {2}", scale.x, scale.y, scale.z);
			}

			auto scriptComponent = entity["ScriptComponent"];
			if ( scriptComponent )
			{
				auto moduleName = scriptComponent["ModuleName"].as<String>();
				deserializedEntity.AddComponent<ScriptComponent>(moduleName);

				SE_CORE_INFO("  Script Module: {0}", moduleName);

				if ( ScriptEngine::ModuleExists(moduleName) )
				{
					auto storedFields = scriptComponent["StoredFields"];
					if ( storedFields )
					{
						for ( auto field : storedFields )
						{
							// TODO: Look over overwritten variables
							auto name = field["Name"].as<String>();
							FieldType type = static_cast<FieldType>(field["Type"].as<uint32_t>());
							EntityInstanceData &data = ScriptEngine::GetEntityInstanceData(m_Scene.GetUUID(), uuid);
							auto &moduleFieldMap = data.ModuleFieldMap;
							auto &publicFields = moduleFieldMap[moduleName];
							if ( publicFields.find(name) == publicFields.end() )
							{
								PublicField pf = { name, type };
								publicFields.emplace(name, Move(pf));
							}
							auto dataNode = field["Data"];
							switch ( type )
							{
							case FieldType::Float:
							{
								publicFields.at(name).SetStoredValue(dataNode.as<float>());
								break;
							}
							case FieldType::Int:
							{
								publicFields.at(name).SetStoredValue(dataNode.as<int32_t>());
								break;
							}
							case FieldType::UnsignedInt:
							{
								publicFields.at(name).SetStoredValue(dataNode.as<uint32_t>());
								break;
							}
							case FieldType::String:
							{
								SE_CORE_ASSERT(false, "Unimplemented");
								break;
							}
							case FieldType::Vec2:
							{
								publicFields.at(name).SetStoredValue(dataNode.as<Vector2f>());
								break;
							}
							case FieldType::Vec3:
							{
								publicFields.at(name).SetStoredValue(dataNode.as<Vector3f>());
								break;
							}
							case FieldType::Vec4:
							{
								publicFields.at(name).SetStoredValue(dataNode.as<Vector4f>());
								break;
							}
							default:
								break;
							}
						}
					}
				}
			}

			auto meshComponent = entity["MeshComponent"];
			if ( meshComponent )
			{
				auto meshPath = meshComponent["AssetPath"].as<String>();
				// TEMP (because script creates mesh component...)
				if ( !deserializedEntity.HasComponent<MeshComponent>() )
					deserializedEntity.AddComponent<MeshComponent>(Shared<Mesh>::Create(meshPath));

				SE_CORE_INFO("  Mesh Asset Path: {0}", meshPath);
			}

			auto cameraComponent = entity["CameraComponent"];
			if ( cameraComponent )
			{
				auto &component = deserializedEntity.AddComponent<CameraComponent>();
				component.Camera = Shared<SceneCamera>::Create();
				component.Primary = cameraComponent["Primary"].as<bool>();

				SE_CORE_INFO("  Primary Camera: {0}", component.Primary);
			}

			auto spriteRendererComponent = entity["SpriteRendererComponent"];
			if ( spriteRendererComponent )
			{
				auto &component = deserializedEntity.AddComponent<SpriteRendererComponent>();
				component.Color = spriteRendererComponent["Color"].as<Vector4f>();
				component.TilingFactor = spriteRendererComponent["TilingFactor"].as<float>();
			}

			auto rigidBody2DComponent = entity["RigidBody2DComponent"];
			if ( rigidBody2DComponent )
			{
				auto &component = deserializedEntity.AddComponent<RigidBody2DComponent>();
				component.BodyType = static_cast<RigidBody2DComponent::Type>(rigidBody2DComponent["BodyType"].as<int>());
				component.FixedRotation = rigidBody2DComponent["FixedRotation"] ? rigidBody2DComponent["FixedRotation"].as<bool>() : false;
			}

			auto boxCollider2DComponent = entity["BoxCollider2DComponent"];
			if ( boxCollider2DComponent )
			{
				auto &component = deserializedEntity.AddComponent<BoxCollider2DComponent>();
				component.Offset = boxCollider2DComponent["Offset"].as<Vector2f>();
				component.Size = boxCollider2DComponent["Size"].as<Vector2f>();
				component.Density = boxCollider2DComponent["Density"] ? boxCollider2DComponent["Density"].as<float>() : 1.0f;
				component.Friction = boxCollider2DComponent["Friction"] ? boxCollider2DComponent["Friction"].as<float>() : 1.0f;
			}

			auto circleCollider2DComponent = entity["CircleCollider2DComponent"];
			if ( circleCollider2DComponent )
			{
				auto &component = deserializedEntity.AddComponent<CircleCollider2DComponent>();
				component.Offset = circleCollider2DComponent["Offset"].as<Vector2f>();
				component.Radius = circleCollider2DComponent["Radius"].as<float>();
				component.Density = circleCollider2DComponent["Density"] ? circleCollider2DComponent["Density"].as<float>() : 1.0f;
				component.Friction = circleCollider2DComponent["Friction"] ? circleCollider2DComponent["Friction"].as<float>() : 1.0f;
			}

			auto rigidBody3DComponent = entity["RigidBody3DComponent"];
			if ( rigidBody3DComponent )
			{
				auto &component = deserializedEntity.AddComponent<RigidBody3DComponent>();
				component.BodyType = static_cast<RigidBody3DComponent::Type>(rigidBody3DComponent["BodyType"].as<int>());
			}

			auto boxCollider3DComponent = entity["BoxCollider3DComponent"];
			if ( boxCollider3DComponent )
			{
				auto &component = deserializedEntity.AddComponent<BoxCollider3DComponent>();
				component.Offset = boxCollider3DComponent["Offset"].as<Vector3f>();
				component.Size = boxCollider3DComponent["Size"].as<Vector3f>();
				component.Density = boxCollider3DComponent["Density"] ? boxCollider3DComponent["Density"].as<float>() : 1.0f;
				component.Friction = boxCollider3DComponent["Friction"] ? boxCollider3DComponent["Friction"].as<float>() : 1.0f;
			}

			auto sphereCollider3DComponent = entity["SphereCollider3DComponent"];
			if ( sphereCollider3DComponent )
			{
				auto &component = deserializedEntity.AddComponent<SphereCollider3DComponent>();
				component.Offset = sphereCollider3DComponent["Offset"].as<Vector3f>();
				component.Radius = sphereCollider3DComponent["Radius"].as<float>();
				component.Density = sphereCollider3DComponent["Density"] ? sphereCollider3DComponent["Density"].as<float>() : 1.0f;
				component.Friction = sphereCollider3DComponent["Friction"] ? sphereCollider3DComponent["Friction"].as<float>() : 1.0f;
			}

		}
	}
	return true;
}

bool SceneSerializer::DeserializeRuntime(const Filepath &filepath)
{
	// Not implemented
	SE_CORE_ASSERT(false);
	return false;
}
}
