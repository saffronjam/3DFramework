#include "SaffronPCH.h"

#include <mono/jit/jit.h>

#include "Saffron/Entity/EntityComponents.h"
#include "Saffron/Entity/Entity.h"
#include "Saffron/Script/ScriptEngine.h"
#include "Saffron/Script/ScriptEngineRegistry.h"
#include "Saffron/Script/ScriptWrappers.h"

namespace Se
{
#define ADD_INTERNAL_CALL_MONO(className, functionName) RegisterFunctionBuffer << "Se." << #className << "::" << #functionName << "_Native";\
	mono_add_internal_call(RegisterFunctionBuffer.str().c_str(), Script::Saffron_ ## className ## _ ## functionName);\
	RegisterFunctionBuffer.str("");\
	RegisterFunctionBuffer.clear();

UnorderedMap<MonoType*, Function<bool(Entity&)>> s_HasComponentFuncs;
UnorderedMap<MonoType*, Function<void(Entity&)>> s_CreateComponentFuncs;

extern MonoImage* s_CoreAssemblyImage;

#define ComponentRegisterType(Type) \
	{\
		MonoType* type = mono_reflection_type_from_name(const_cast<char*>("Se." #Type), s_CoreAssemblyImage);\
		if (type) {\
			Uint32 id = mono_type_get_type(type);\
			s_HasComponentFuncs[type] = [](Entity& entity) { return entity.HasComponent<Type>(); };\
			s_CreateComponentFuncs[type] = [](Entity& entity) { entity.AddComponent<Type>(); };\
		} else {\
			Log::CoreError("No C# component class found for " #Type "!");\
		}\
	}

static void InitComponentTypes()
{
	ComponentRegisterType(TagComponent);
	ComponentRegisterType(TransformComponent);
	ComponentRegisterType(MeshComponent);
	ComponentRegisterType(ScriptComponent);
	ComponentRegisterType(CameraComponent);
	ComponentRegisterType(SpriteRendererComponent);
	ComponentRegisterType(RigidBody2DComponent);
	ComponentRegisterType(BoxCollider2DComponent);
	ComponentRegisterType(CircleCollider2DComponent);
	ComponentRegisterType(RigidBody3DComponent);
	ComponentRegisterType(BoxCollider3DComponent);
	ComponentRegisterType(SphereCollider3DComponent);
}

void ScriptEngineRegistry::RegisterAll()
{
	InitComponentTypes();

	OutputStringStream RegisterFunctionBuffer;

	ADD_INTERNAL_CALL_MONO(Entity, GetTransform);
	ADD_INTERNAL_CALL_MONO(Entity, SetTransform);
	ADD_INTERNAL_CALL_MONO(Entity, CreateComponent);
	ADD_INTERNAL_CALL_MONO(Entity, HasComponent);
	ADD_INTERNAL_CALL_MONO(Entity, GetEntity);

	ADD_INTERNAL_CALL_MONO(MeshComponent, GetMesh);
	ADD_INTERNAL_CALL_MONO(MeshComponent, SetMesh);

	ADD_INTERNAL_CALL_MONO(Input, IsKeyPressed);
	ADD_INTERNAL_CALL_MONO(Input, IsMouseButtonPressed);

	ADD_INTERNAL_CALL_MONO(Texture2D, Constructor);
	ADD_INTERNAL_CALL_MONO(Texture2D, Destructor);
	ADD_INTERNAL_CALL_MONO(Texture2D, SetData);

	ADD_INTERNAL_CALL_MONO(Material, Destructor);
	ADD_INTERNAL_CALL_MONO(Material, SetFloat);
	ADD_INTERNAL_CALL_MONO(Material, SetTexture);

	ADD_INTERNAL_CALL_MONO(MaterialInstance, Destructor);
	ADD_INTERNAL_CALL_MONO(MaterialInstance, SetFloat);
	ADD_INTERNAL_CALL_MONO(MaterialInstance, SetVector3);
	ADD_INTERNAL_CALL_MONO(MaterialInstance, SetVector4);
	ADD_INTERNAL_CALL_MONO(MaterialInstance, SetTexture);

	ADD_INTERNAL_CALL_MONO(Mesh, Constructor);
	ADD_INTERNAL_CALL_MONO(Mesh, Destructor);
	ADD_INTERNAL_CALL_MONO(Mesh, GetMaterial);
	ADD_INTERNAL_CALL_MONO(Mesh, GetMaterialByIndex);
	ADD_INTERNAL_CALL_MONO(Mesh, GetMaterialCount);

	ADD_INTERNAL_CALL_MONO(MeshFactory, CreatePlane);

	ADD_INTERNAL_CALL_MONO(Camera, Constructor);
	ADD_INTERNAL_CALL_MONO(Camera, Destructor);
	ADD_INTERNAL_CALL_MONO(Camera, GetProjectionMode);
	ADD_INTERNAL_CALL_MONO(Camera, SetProjectionMode);

	ADD_INTERNAL_CALL_MONO(CameraComponent, GetCamera);
	ADD_INTERNAL_CALL_MONO(CameraComponent, SetCamera);

	ADD_INTERNAL_CALL_MONO(ScriptComponent, GetModuleName);
	ADD_INTERNAL_CALL_MONO(ScriptComponent, SetModuleName);

	ADD_INTERNAL_CALL_MONO(RigidBody2DComponent, ApplyLinearImpulse);
	ADD_INTERNAL_CALL_MONO(RigidBody2DComponent, GetLinearVelocity);
	ADD_INTERNAL_CALL_MONO(RigidBody2DComponent, SetLinearVelocity);

	ADD_INTERNAL_CALL_MONO(Collider2DComponent, GetOffset);
	ADD_INTERNAL_CALL_MONO(Collider2DComponent, SetOffset);
	ADD_INTERNAL_CALL_MONO(Collider2DComponent, GetDensity);
	ADD_INTERNAL_CALL_MONO(Collider2DComponent, SetDensity);
	ADD_INTERNAL_CALL_MONO(Collider2DComponent, GetFriction);
	ADD_INTERNAL_CALL_MONO(Collider2DComponent, SetFriction);

	ADD_INTERNAL_CALL_MONO(BoxCollider2DComponent, GetSize);
	ADD_INTERNAL_CALL_MONO(BoxCollider2DComponent, SetSize);

	ADD_INTERNAL_CALL_MONO(CircleCollider2DComponent, GetRadius);
	ADD_INTERNAL_CALL_MONO(CircleCollider2DComponent, SetRadius);

	ADD_INTERNAL_CALL_MONO(RigidBody3DComponent, ApplyLinearImpulse);
	ADD_INTERNAL_CALL_MONO(RigidBody3DComponent, GetLinearVelocity);
	ADD_INTERNAL_CALL_MONO(RigidBody3DComponent, SetLinearVelocity);

	ADD_INTERNAL_CALL_MONO(Collider3DComponent, GetOffset);
	ADD_INTERNAL_CALL_MONO(Collider3DComponent, SetOffset);
	ADD_INTERNAL_CALL_MONO(Collider3DComponent, GetDensity);
	ADD_INTERNAL_CALL_MONO(Collider3DComponent, SetDensity);
	ADD_INTERNAL_CALL_MONO(Collider3DComponent, GetFriction);
	ADD_INTERNAL_CALL_MONO(Collider3DComponent, SetFriction);

	ADD_INTERNAL_CALL_MONO(BoxCollider3DComponent, GetSize);
	ADD_INTERNAL_CALL_MONO(BoxCollider3DComponent, SetSize);

	ADD_INTERNAL_CALL_MONO(SphereCollider3DComponent, GetRadius);
	ADD_INTERNAL_CALL_MONO(SphereCollider3DComponent, SetRadius);
}
}
