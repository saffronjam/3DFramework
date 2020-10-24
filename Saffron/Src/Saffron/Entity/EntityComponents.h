#pragma once

#include "Saffron/Core/UUID.h"
#include "Saffron/Physics/PhysicsWorld2D.h"
#include "Saffron/Renderer/Mesh.h"
#include "Saffron/Scene/SceneCamera.h"

namespace Se
{
struct IDComponent
{
	UUID ID = 0;
};

struct SceneComponent
{
	UUID SceneID;
};

struct TagComponent
{
	String Tag;

	TagComponent() = default;
	TagComponent(const TagComponent &other) = default;
	explicit TagComponent(String tag)
		: Tag(std::move(tag))
	{
	}

	operator String &() { return Tag; }
	operator const String &() const { return Tag; }
};

struct TransformComponent
{
	Matrix4f Transform;

	TransformComponent() = default;
	TransformComponent(const TransformComponent &other) = default;
	explicit TransformComponent(const Matrix4f &transform)
		: Transform(transform)
	{
	}

	operator Matrix4f &() { return Transform; }
	operator const Matrix4f &() const { return Transform; }
};

struct MeshComponent
{
	Shared<Mesh> Mesh;

	MeshComponent() = default;
	MeshComponent(const MeshComponent &other) = default;
	explicit MeshComponent(const Shared<Se::Mesh> &mesh)
		: Mesh(mesh)
	{
	}

	operator Shared<Se::Mesh>() const { return Mesh; }
};

struct ScriptComponent
{
	String ModuleName;
	String NamespaceName;
	String ClassName;

	ScriptComponent() = default;
	ScriptComponent(const ScriptComponent &other) = default;
	explicit ScriptComponent(String moduleName)
		: ModuleName(std::move(moduleName))
	{
		if ( ModuleName.find('.') != String::npos )
		{
			NamespaceName = ModuleName.substr(0, ModuleName.find_last_of('.'));
			ClassName = ModuleName.substr(ModuleName.find_last_of('.') + 1);
		}
		else
		{
			ClassName = ModuleName;
		}
	}

	void ChangeModule(String moduleName)
	{
		ModuleName = std::move(moduleName);
		if ( ModuleName.find('.') != String::npos )
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
	Shared<SceneCamera> Camera;
	bool Primary = true;

	CameraComponent() = default;
	CameraComponent(const CameraComponent &other) = default;

	operator Shared<SceneCamera>() { return Camera; }
};

struct SpriteRendererComponent
{
	Vector4f Color = { 1.0f, 1.0f, 1.0f, 1.0f };
	Shared<Texture2D> Texture;
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
	Vector2f Offset = { 0.0f,0.0f };

	float Density = 1.0f;
	float Friction = 1.0f;

	// Storage for runtime
	void *RuntimeFixture = nullptr;
};

struct BoxCollider2DComponent : Collider2DComponent
{
	Vector2f Size = { 1.0f, 1.0f };

	BoxCollider2DComponent() = default;
	BoxCollider2DComponent(const BoxCollider2DComponent &other) = default;
};

struct CircleCollider2DComponent : Collider2DComponent
{
	float Radius = 1.0f;

	CircleCollider2DComponent() = default;
	CircleCollider2DComponent(const CircleCollider2DComponent &other) = default;
};

struct PhysicsWorld2DComponent
{
	PhysicsWorld2D World;

	PhysicsWorld2DComponent(Scene &scene)
		: World(scene)
	{
	}
};
}

