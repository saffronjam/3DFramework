#pragma once

#include "Saffron/Base.h"
#include "Saffron/Editor/EditorCamera.h"
#include "Saffron/Renderer/LightType.h"
#include "Saffron/Renderer/Mesh.h"
#include "Saffron/Scene/SceneEnvironment.h"
#include "Saffron/Scene/SceneCamera.h"

namespace Se
{
struct IDComponent
{
	UUID ID = 0;
};

struct TagComponent
{
	String Tag;

	TagComponent() = default;
	TagComponent(const TagComponent& other) = default;

	explicit TagComponent(String tag) :
		Tag(Move(tag))
	{
	}

	operator String&() { return Tag; }

	operator const String&() const { return Tag; }
};

struct TransformComponent
{
	Matrix4f Transform;

	TransformComponent() :
		Transform(1)
	{
	}

	TransformComponent(Matrix4f transform) :
		Transform(Move(transform))
	{
	}

	TransformComponent(const TransformComponent& other) = default;

	operator Matrix4f&() { return Transform; }

	operator const Matrix4f&() const { return Transform; }
};

struct MeshComponent
{
	Shared<Mesh> Mesh;

	MeshComponent() = default;
	MeshComponent(const MeshComponent& other) = default;

	explicit MeshComponent(const Shared<Se::Mesh>& mesh) :
		Mesh(mesh)
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
	ScriptComponent(const ScriptComponent& other) = default;

	explicit ScriptComponent(String moduleName) :
		ModuleName(Move(moduleName))
	{
		if (ModuleName.find('.') != String::npos)
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
		ModuleName = Move(moduleName);
		if (ModuleName.find('.') != String::npos)
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

	Shared<Mesh> CameraMesh;
	bool DrawMesh = true;
	bool DrawFrustum = false;

	CameraComponent() :
		Camera(CreateShared<SceneCamera>()),
		CameraMesh(CreateShared<Mesh>("Camera.fbx"))
	{
		CameraMesh->GetMaterial()->Set<Vector3f>("u_AlbedoColor", {0.0f, 0.0f, 0.45});
	}

	CameraComponent(const CameraComponent& other) = default;

	operator Shared<SceneCamera>() const { return Camera; }
};

struct SpriteRendererComponent
{
	Vector4f Color = {1.0f, 1.0f, 1.0f, 1.0f};
	Shared<Texture2D> Texture;
	float TilingFactor = 1.0f;

	SpriteRendererComponent() = default;
	SpriteRendererComponent(const SpriteRendererComponent& other) = default;
};

struct RigidBody2DComponent
{
	enum class Type { Static, Dynamic, Kinematic } BodyType;

	bool FixedRotation = false;

	// Storage for runtime
	void* RuntimeBody = nullptr;

	RigidBody2DComponent() = default;
	RigidBody2DComponent(const RigidBody2DComponent& other) = default;
};

struct Collider2DComponent
{
	Vector2f Offset = {0.0f, 0.0f};

	float Density = 1.0f;
	float Friction = 1.0f;
	bool DrawBounding = false;

	// Storage for runtime
	void* RuntimeFixture = nullptr;
};

struct BoxCollider2DComponent : Collider2DComponent
{
	Vector2f Size = {1.0f, 1.0f};

	BoxCollider2DComponent() = default;
	BoxCollider2DComponent(const BoxCollider2DComponent& other) = default;
};

struct CircleCollider2DComponent : Collider2DComponent
{
	float Radius = 1.0f;

	CircleCollider2DComponent() = default;
	CircleCollider2DComponent(const CircleCollider2DComponent& other) = default;
};

struct RigidBody3DComponent
{
	enum class Type { Static, Dynamic, Kinematic } BodyType;

	// Storage for runtime
	void* RuntimeBody = nullptr;

	RigidBody3DComponent() = default;
	RigidBody3DComponent(const RigidBody3DComponent& other) = default;
};


struct Collider3DComponent
{
	Vector3f Offset = {0.0f, 0.0f, 0.0f};

	float Density = 1.0f;
	float Friction = 1.0f;
	bool DrawBounding = false;

	// Storage for runtime
	void* RuntimeFixture = nullptr;
};


struct BoxCollider3DComponent : Collider3DComponent
{
	Vector3f Size = {1.0f, 1.0f, 1.0f};

	BoxCollider3DComponent() = default;
	BoxCollider3DComponent(const BoxCollider3DComponent& other) = default;
};

struct SphereCollider3DComponent : Collider3DComponent
{
	float Radius = 0.5f;

	SphereCollider3DComponent() = default;
	SphereCollider3DComponent(const SphereCollider3DComponent& other) = default;
};

/////////////////////////////////////////////////////////
/// Lights 
/////////////////////////////////////////////////////////

struct DirectionalLightComponent
{
	Vector3f Radiance = {1.0f, 1.0f, 1.0f};
	float Intensity = 1.0f;
	bool CastShadows = true;
	bool SoftShadows = true;
	float LightSize = 0.5f; // For PCSS
};

struct SkylightComponent
{
	Shared<SceneEnvironment> SceneEnvironment;
	float Intensity = 1.0f;
	float Angle = 0.0f;

	SkylightComponent() = default;

	explicit SkylightComponent(Shared<Se::SceneEnvironment>& sceneEnvironment) :
		SceneEnvironment(sceneEnvironment)
	{
	}
};
}
