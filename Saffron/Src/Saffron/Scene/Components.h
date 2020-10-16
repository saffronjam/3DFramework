#pragma once

#include "Saffron/Core/UUID.h"
#include "Saffron/Renderer/Mesh.h"
#include "Saffron/Scene/SceneCamera.h"

namespace Se
{
struct IDComponent
{
	UUID ID = 0;
};

struct TagComponent
{
	std::string Tag;

	TagComponent() = default;
	TagComponent(const TagComponent &other) = default;
	explicit TagComponent(std::string tag)
		: Tag(std::move(tag))
	{
	}

	operator std::string &() { return Tag; }
	operator const std::string &() const { return Tag; }
};

struct TransformComponent
{
	glm::mat4 Transform;

	TransformComponent() = default;
	TransformComponent(const TransformComponent &other) = default;
	explicit TransformComponent(const glm::mat4 &transform)
		: Transform(transform)
	{
	}

	operator glm::mat4 &() { return Transform; }
	operator const glm::mat4 &() const { return Transform; }
};

struct MeshComponent
{
	Ref<Mesh> Mesh;

	MeshComponent() = default;
	MeshComponent(const MeshComponent &other) = default;
	explicit MeshComponent(const Ref<Se::Mesh> &mesh)
		: Mesh(mesh)
	{
	}

	operator Ref<Se::Mesh>() const { return Mesh; }
};

struct ScriptComponent
{
	std::string ModuleName;
	std::string NamespaceName;
	std::string ClassName;

	ScriptComponent() = default;
	ScriptComponent(const ScriptComponent &other) = default;
	explicit ScriptComponent(std::string moduleName)
		: ModuleName(std::move(moduleName))
	{
		if ( ModuleName.find('.') != std::string::npos )
		{
			NamespaceName = ModuleName.substr(0, ModuleName.find_last_of('.'));
			ClassName = ModuleName.substr(ModuleName.find_last_of('.') + 1);
		}
		else
		{
			ClassName = ModuleName;
		}
	}

	void ChangeModule(std::string moduleName)
	{
		ModuleName = std::move(moduleName);
		if ( ModuleName.find('.') != std::string::npos )
		{
			NamespaceName = ModuleName.substr(0, ModuleName.find_last_of('.'));
			ClassName = ModuleName.substr(ModuleName.find_last_of('.') + 1);
		}
		else
		{
			ClassName = ModuleName;
		}
	}
};

struct CameraComponent
{
	Ref<SceneCamera> Camera;
	bool Primary = true;

	CameraComponent() = default;
	CameraComponent(const CameraComponent &other) = default;

	operator Ref<SceneCamera>() { return Camera; }
};

struct SpriteRendererComponent
{
	glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
	Ref<Texture2D> Texture;
	float TilingFactor = 1.0f;

	SpriteRendererComponent() = default;
	SpriteRendererComponent(const SpriteRendererComponent &other) = default;
};

struct RigidBody2DComponent
{
	enum class Type { Static, Dynamic, Kinematic };
	Type BodyType;
	bool FixedRotation = false;

	// Storage for runtime
	void *RuntimeBody = nullptr;

	RigidBody2DComponent() = default;
	RigidBody2DComponent(const RigidBody2DComponent &other) = default;
};

struct Collider2DComponent
{
	glm::vec2 Offset = { 0.0f,0.0f };

	float Density = 1.0f;
	float Friction = 1.0f;

	// Storage for runtime
	void *RuntimeFixture = nullptr;
};

struct BoxCollider2DComponent : Collider2DComponent
{
	glm::vec2 Size = { 1.0f, 1.0f };

	BoxCollider2DComponent() = default;
	BoxCollider2DComponent(const BoxCollider2DComponent &other) = default;
};

struct CircleCollider2DComponent : Collider2DComponent
{
	float Radius = 1.0f;

	CircleCollider2DComponent() = default;
	CircleCollider2DComponent(const CircleCollider2DComponent &other) = default;
};
}

