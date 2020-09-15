#include "Saffron/SaffronPCH.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

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
		MonoType* type = mono_reflection_type_from_name("Saffron." #Type, s_CoreAssemblyImage);\
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

	mono_add_internal_call("Saffron.Entity::GetTransform_Native", Script::Saffron_Entity_GetTransform);
	mono_add_internal_call("Saffron.Entity::SetTransform_Native", Script::Saffron_Entity_SetTransform);
	mono_add_internal_call("Saffron.Entity::CreateComponent_Native", Script::Saffron_Entity_CreateComponent);
	mono_add_internal_call("Saffron.Entity::HasComponent_Native", Script::Saffron_Entity_HasComponent);
	mono_add_internal_call("Saffron.Entity::FindEntityByTag_Native", Script::Saffron_Entity_FindEntityByTag);

	mono_add_internal_call("Saffron.MeshComponent::GetMesh_Native", Script::Saffron_MeshComponent_GetMesh);
	mono_add_internal_call("Saffron.MeshComponent::SetMesh_Native", Script::Saffron_MeshComponent_SetMesh);

	mono_add_internal_call("Saffron.RigidBody2DComponent::ApplyLinearImpulse_Native", Script::Saffron_RigidBody2DComponent_ApplyLinearImpulse);
	mono_add_internal_call("Saffron.RigidBody2DComponent::GetLinearVelocity_Native", Script::Saffron_RigidBody2DComponent_GetLinearVelocity);
	mono_add_internal_call("Saffron.RigidBody2DComponent::SetLinearVelocity_Native", Script::Saffron_RigidBody2DComponent_SetLinearVelocity);

	mono_add_internal_call("Saffron.Input::IsKeyPressed_Native", Script::Saffron_Input_IsKeyPressed);

	mono_add_internal_call("Saffron.Texture2D::Constructor_Native", Script::Saffron_Texture2D_Constructor);
	mono_add_internal_call("Saffron.Texture2D::Destructor_Native", Script::Saffron_Texture2D_Destructor);
	mono_add_internal_call("Saffron.Texture2D::SetData_Native", Script::Saffron_Texture2D_SetData);

	mono_add_internal_call("Saffron.Material::Destructor_Native", Script::Saffron_Material_Destructor);
	mono_add_internal_call("Saffron.Material::SetFloat_Native", Script::Saffron_Material_SetFloat);
	mono_add_internal_call("Saffron.Material::SetTexture_Native", Script::Saffron_Material_SetTexture);

	mono_add_internal_call("Saffron.MaterialInstance::Destructor_Native", Script::Saffron_MaterialInstance_Destructor);
	mono_add_internal_call("Saffron.MaterialInstance::SetFloat_Native", Script::Saffron_MaterialInstance_SetFloat);
	mono_add_internal_call("Saffron.MaterialInstance::SetVector3_Native", Script::Saffron_MaterialInstance_SetVector3);
	mono_add_internal_call("Saffron.MaterialInstance::SetVector4_Native", Script::Saffron_MaterialInstance_SetVector4);
	mono_add_internal_call("Saffron.MaterialInstance::SetTexture_Native", Script::Saffron_MaterialInstance_SetTexture);

	mono_add_internal_call("Saffron.Mesh::Constructor_Native", Script::Saffron_Mesh_Constructor);
	mono_add_internal_call("Saffron.Mesh::Destructor_Native", Script::Saffron_Mesh_Destructor);
	mono_add_internal_call("Saffron.Mesh::GetMaterial_Native", Script::Saffron_Mesh_GetMaterial);
	mono_add_internal_call("Saffron.Mesh::GetMaterialByIndex_Native", Script::Saffron_Mesh_GetMaterialByIndex);
	mono_add_internal_call("Saffron.Mesh::GetMaterialCount_Native", Script::Saffron_Mesh_GetMaterialCount);

	mono_add_internal_call("Saffron.MeshFactory::CreatePlane_Native", Script::Saffron_MeshFactory_CreatePlane);

	// static bool IsKeyPressed(KeyCode key) { return s_Instance->IsKeyPressedImpl(key); }
	// 
	// static bool IsMouseButtonPressed(MouseCode button) { return s_Instance->IsMouseButtonPressedImpl(button); }
	// static std::pair<float, float> GetMousePosition() { return s_Instance->GetMousePositionImpl(); }
	// static float GetMouseX() { return s_Instance->GetMouseXImpl(); }
	// static float GetMouseY() { return s_Instance->GetMouseYImpl(); }
}

}