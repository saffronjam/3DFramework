#include "SaffronPCH.h"

#include <mono/jit/jit.h>

#include "Saffron/Scene/Components.h"
#include "Saffron/Scene/Entity.h"
#include "Saffron/Script/ScriptEngine.h"
#include "Saffron/Script/ScriptEngineRegistry.h"
#include "Saffron/Script/ScriptWrappers.h"

namespace Se
{
std::unordered_map<MonoType *, std::function<bool(Entity &)>> s_HasComponentFuncs;
std::unordered_map<MonoType *, std::function<void(Entity &)>> s_CreateComponentFuncs;

extern MonoImage *s_CoreAssemblyImage;

#define Component_RegisterType(Type) \
	{\
		MonoType* type = mono_reflection_type_from_name("Se." #Type, s_CoreAssemblyImage);\
		if (type) {\
			Uint32 id = mono_type_get_type(type);\
			s_HasComponentFuncs[type] = [](Entity& entity) { return entity.HasComponent<Type>(); };\
			s_CreateComponentFuncs[type] = [](Entity& entity) { entity.AddComponent<Type>(); };\
		} else {\
			SE_CORE_ERROR("No C# component class found for " #Type "!");\
		}\
	}

static void InitComponentTypes()
{
	Component_RegisterType(TagComponent);
	Component_RegisterType(TransformComponent);
	Component_RegisterType(MeshComponent);
	Component_RegisterType(ScriptComponent);
	Component_RegisterType(CameraComponent);
	Component_RegisterType(SpriteRendererComponent);
	Component_RegisterType(RigidBody2DComponent);
	Component_RegisterType(BoxCollider2DComponent);
}

void ScriptEngineRegistry::RegisterAll()
{
	InitComponentTypes();

	mono_add_internal_call("Se.Entity::GetTransform_Native", Script::Saffron_Entity_GetTransform);
	mono_add_internal_call("Se.Entity::SetTransform_Native", Script::Saffron_Entity_SetTransform);
	mono_add_internal_call("Se.Entity::CreateComponent_Native", Script::Saffron_Entity_CreateComponent);
	mono_add_internal_call("Se.Entity::HasComponent_Native", Script::Saffron_Entity_HasComponent);
	mono_add_internal_call("Se.Entity::FindEntityByTag_Native", Script::Saffron_Entity_FindEntityByTag);

	mono_add_internal_call("Se.MeshComponent::GetMesh_Native", Script::Saffron_MeshComponent_GetMesh);
	mono_add_internal_call("Se.MeshComponent::SetMesh_Native", Script::Saffron_MeshComponent_SetMesh);

	mono_add_internal_call("Se.RigidBody2DComponent::ApplyLinearImpulse_Native", Script::Saffron_RigidBody2DComponent_ApplyLinearImpulse);
	mono_add_internal_call("Se.RigidBody2DComponent::GetLinearVelocity_Native", Script::Saffron_RigidBody2DComponent_GetLinearVelocity);
	mono_add_internal_call("Se.RigidBody2DComponent::SetLinearVelocity_Native", Script::Saffron_RigidBody2DComponent_SetLinearVelocity);

	mono_add_internal_call("Se.Input::IsKeyPressed_Native", Script::Saffron_Input_IsKeyPressed);

	mono_add_internal_call("Se.Texture2D::Constructor_Native", Script::Saffron_Texture2D_Constructor);
	mono_add_internal_call("Se.Texture2D::Destructor_Native", Script::Saffron_Texture2D_Destructor);
	mono_add_internal_call("Se.Texture2D::SetData_Native", Script::Saffron_Texture2D_SetData);

	mono_add_internal_call("Se.Material::Destructor_Native", Script::Saffron_Material_Destructor);
	mono_add_internal_call("Se.Material::SetFloat_Native", Script::Saffron_Material_SetFloat);
	mono_add_internal_call("Se.Material::SetTexture_Native", Script::Saffron_Material_SetTexture);

	mono_add_internal_call("Se.MaterialInstance::Destructor_Native", Script::Saffron_MaterialInstance_Destructor);
	mono_add_internal_call("Se.MaterialInstance::SetFloat_Native", Script::Saffron_MaterialInstance_SetFloat);
	mono_add_internal_call("Se.MaterialInstance::SetVector3_Native", Script::Saffron_MaterialInstance_SetVector3);
	mono_add_internal_call("Se.MaterialInstance::SetVector4_Native", Script::Saffron_MaterialInstance_SetVector4);
	mono_add_internal_call("Se.MaterialInstance::SetTexture_Native", Script::Saffron_MaterialInstance_SetTexture);

	mono_add_internal_call("Se.Mesh::Constructor_Native", Script::Saffron_Mesh_Constructor);
	mono_add_internal_call("Se.Mesh::Destructor_Native", Script::Saffron_Mesh_Destructor);
	mono_add_internal_call("Se.Mesh::GetMaterial_Native", Script::Saffron_Mesh_GetMaterial);
	mono_add_internal_call("Se.Mesh::GetMaterialByIndex_Native", Script::Saffron_Mesh_GetMaterialByIndex);
	mono_add_internal_call("Se.Mesh::GetMaterialCount_Native", Script::Saffron_Mesh_GetMaterialCount);

	mono_add_internal_call("Se.MeshFactory::CreatePlane_Native", Script::Saffron_MeshFactory_CreatePlane);

	mono_add_internal_call("Se.Camera::Constructor_Native", Script::Saffron_SceneCamera_Constructor);
	mono_add_internal_call("Se.Camera::Destructor_Native", Script::Saffron_SceneCamera_Destructor);

	mono_add_internal_call("Se.CameraComponent::GetCamera_Native", Script::Saffron_CameraComponent_GetCamera);
	mono_add_internal_call("Se.CameraComponent::SetCamera_Native", Script::Saffron_CameraComponent_SetCamera);

	// static bool IsKeyPressed(KeyCode key) { return s_Instance->IsKeyPressedImpl(key); }
	// 
	// static bool IsMouseButtonPressed(MouseCode button) { return s_Instance->IsMouseButtonPressedImpl(button); }
	// static std::pair<float, float> GetMousePosition() { return s_Instance->GetMousePositionImpl(); }
	// static float GetMouseX() { return s_Instance->GetMouseXImpl(); }
	// static float GetMouseY() { return s_Instance->GetMouseYImpl(); }
}

}