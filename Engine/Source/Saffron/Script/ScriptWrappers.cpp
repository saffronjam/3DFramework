#include "SaffronPCH.h"

#include <box2d/box2d.h>
#include <reactphysics3d/reactphysics3d.h>
#include <mono/jit/jit.h>

#include "Saffron/Input/Keyboard.h"
#include "Saffron/Input/Mouse.h"
#include "Saffron/Entity/Entity.h"
#include "Saffron/Script/ScriptWrappers.h"

namespace Se
{
///////////////////////////////////////////////////////////////////////////
/// Externs
///////////////////////////////////////////////////////////////////////////

extern UnorderedMap<MonoType*, Function<bool(Entity&)>> s_HasComponentFuncs;
extern UnorderedMap<MonoType*, Function<void(Entity&)>> s_CreateComponentFuncs;

namespace Script
{
enum class ComponentID
{
	None = 0,
	Transform = 1,
	Mesh = 2,
	Script = 3,
	SpriteRenderer = 4
};


////////////////////////////////////////////////////////////////
// Helper functions
////////////////////////////////////////////////////////////////

Entity GetEntityFromActiveScene(UUID entityID)
{
	Shared<Scene> scene = ScriptEngine::GetCurrentSceneContext();
	SE_CORE_ASSERT(scene, "No active scene!");
	const auto& entityMap = scene->GetEntityMap();
	SE_CORE_ASSERT(entityMap.find(entityID) != entityMap.end(), "Invalid entity ID or entity doesn't exist in scene!");

	return entityMap.at(entityID);
}

template <typename T>
T& GetComponentWithCheck(Entity entity)
{
	SE_CORE_ASSERT(entity.HasComponent<T>());
	return entity.GetComponent<T>();
}

Shared<Scene> GetSceneWithCheck()
{
	Shared<Scene> scene = ScriptEngine::GetCurrentSceneContext();
	SE_CORE_ASSERT(scene, "No active scene!");
	return scene;
}


////////////////////////////////////////////////////////////////
// Input
////////////////////////////////////////////////////////////////

bool Saffron_Input_IsKeyPressed(KeyCode key)
{
	return Keyboard::IsPressed(key);
}

bool Saffron_Input_IsMouseButtonPressed(MouseButtonCode mouseButton)
{
	return Mouse::IsPressed(mouseButton);
}


////////////////////////////////////////////////////////////////
// Entity //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

void Saffron_Entity_GetTransform(Uint64 entityID, Matrix4f* outTransform)
{
	Entity entity = GetEntityFromActiveScene(entityID);
	auto& transformComponent = entity.GetComponent<TransformComponent>();
	memcpy(outTransform, glm::value_ptr(transformComponent.Transform), sizeof(Matrix4f));
}

void Saffron_Entity_SetTransform(Uint64 entityID, Matrix4f* inTransform)
{
	Entity entity = GetEntityFromActiveScene(entityID);
	auto& transformComponent = entity.GetComponent<TransformComponent>();
	memcpy(glm::value_ptr(transformComponent.Transform), inTransform, sizeof(Matrix4f));
}

void Saffron_Entity_CreateComponent(Uint64 entityID, void* type)
{
	Entity entity = GetEntityFromActiveScene(entityID);
	MonoType* monoType = mono_reflection_type_get_type(static_cast<MonoReflectionType*>(type));
	s_CreateComponentFuncs[monoType](entity);
}

bool Saffron_Entity_HasComponent(Uint64 entityID, void* type)
{
	Entity entity = GetEntityFromActiveScene(entityID);
	MonoType* monoType = mono_reflection_type_get_type(static_cast<MonoReflectionType*>(type));
	const bool result = s_HasComponentFuncs[monoType](entity);
	return result;
}

Uint64 Saffron_Entity_GetEntity(MonoString* tag)
{
	Shared<Scene> scene = GetSceneWithCheck();
	Entity entity = scene->GetEntity(mono_string_to_utf8(tag));
	return entity ? entity.GetComponent<IDComponent>().ID : UUID(0);
}

void* Saffron_MeshComponent_GetMesh(Uint64 entityID)
{
	Entity entity = GetEntityFromActiveScene(entityID);
	auto& meshComponent = entity.GetComponent<MeshComponent>();
	return new Shared<Mesh>(meshComponent.Mesh);
}

void Saffron_MeshComponent_SetMesh(Uint64 entityID, Shared<Mesh>* inMesh)
{
	Entity entity = GetEntityFromActiveScene(entityID);
	auto& meshComponent = entity.GetComponent<MeshComponent>();
	meshComponent.Mesh = inMesh ? *inMesh : nullptr;
}

void Saffron_RigidBody2DComponent_ApplyLinearImpulse(Uint64 entityID, Vector2f* impulse, Vector2f* offset, bool wake)
{
	SE_CORE_ASSERT(impulse && offset);
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<RigidBody2DComponent>(entity);
	auto* body = static_cast<b2Body*>(component.RuntimeBody);
	body->ApplyLinearImpulse(*reinterpret_cast<const b2Vec2*>(impulse),
	                         body->GetWorldCenter() + *reinterpret_cast<const b2Vec2*>(offset), wake);
}

void Saffron_RigidBody2DComponent_GetLinearVelocity(Uint64 entityID, Vector2f* outVelocity)
{
	SE_CORE_ASSERT(outVelocity);
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<RigidBody2DComponent>(entity);
	auto* body = static_cast<b2Body*>(component.RuntimeBody);
	const auto& velocity = body->GetLinearVelocity();
	*outVelocity = {velocity.x, velocity.y};
}

void Saffron_RigidBody2DComponent_SetLinearVelocity(Uint64 entityID, Vector2f* velocity)
{
	SE_CORE_ASSERT(velocity);
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<RigidBody2DComponent>(entity);
	auto* body = static_cast<b2Body*>(component.RuntimeBody);
	body->SetLinearVelocity({velocity->x, velocity->y});
}

void Saffron_Collider2DComponent_GetOffset(Uint64 entityID, Vector2f* offset)
{
	SE_CORE_ASSERT(offset);
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<Collider2DComponent>(entity);
	*offset = component.Offset;
}

void Saffron_Collider2DComponent_SetOffset(Uint64 entityID, Vector2f* offset)
{
	SE_CORE_ASSERT(offset);
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<Collider2DComponent>(entity);
	component.Offset = *offset;
}

float Saffron_Collider2DComponent_GetDensity(Uint64 entityID)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<Collider2DComponent>(entity);
	return component.Density;
}

void Saffron_Collider2DComponent_SetDensity(Uint64 entityID, float density)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<Collider2DComponent>(entity);
	component.Density = density;
}

float Saffron_Collider2DComponent_GetFriction(Uint64 entityID)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<Collider2DComponent>(entity);
	return component.Friction;
}

void Saffron_Collider2DComponent_SetFriction(Uint64 entityID, float friction)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<Collider2DComponent>(entity);
	component.Friction = friction;
}

void Saffron_BoxCollider2DComponent_GetSize(Uint64 entityID, Vector2f* size)
{
	SE_CORE_ASSERT(size);
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<BoxCollider2DComponent>(entity);
	*size = component.Size;
}

void Saffron_BoxCollider2DComponent_SetSize(Uint64 entityID, Vector2f* size)
{
	SE_CORE_ASSERT(size);
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<BoxCollider2DComponent>(entity);
	component.Size = *size;
}

float Saffron_CircleCollider2DComponent_GetRadius(Uint64 entityID)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<CircleCollider2DComponent>(entity);
	return component.Radius;
}

void Saffron_CircleCollider2DComponent_SetRadius(Uint64 entityID, float radius)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<CircleCollider2DComponent>(entity);
	component.Radius = radius;
}

void Saffron_RigidBody3DComponent_ApplyLinearImpulse(Uint64 entityID, Vector3f* impulse, Vector3f* offset, bool wake)
{
	SE_CORE_ASSERT(impulse && offset);
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<RigidBody3DComponent>(entity);
	auto* body = static_cast<reactphysics3d::RigidBody*>(component.RuntimeBody);
	body->applyForceAtLocalPosition(*reinterpret_cast<const reactphysics3d::Vector3*>(impulse),
	                                body->getLocalCenterOfMass() + *reinterpret_cast<const reactphysics3d::Vector3*>(
		                                offset));
}

void Saffron_RigidBody3DComponent_GetLinearVelocity(Uint64 entityID, Vector3f* outVelocity)
{
	SE_CORE_ASSERT(outVelocity);
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<RigidBody3DComponent>(entity);
	auto* body = static_cast<reactphysics3d::RigidBody*>(component.RuntimeBody);
	const auto& velocity = body->getLinearVelocity();
	*outVelocity = {velocity.x, velocity.y, velocity.z};
}

void Saffron_RigidBody3DComponent_SetLinearVelocity(Uint64 entityID, Vector3f* velocity)
{
	SE_CORE_ASSERT(velocity);
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<RigidBody3DComponent>(entity);
	auto* body = static_cast<reactphysics3d::RigidBody*>(component.RuntimeBody);
	body->setLinearVelocity({velocity->x, velocity->y, velocity->z});
}

void Saffron_Collider3DComponent_GetOffset(Uint64 entityID, Vector3f* offset)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<Collider3DComponent>(entity);
	*offset = component.Offset;
}

void Saffron_Collider3DComponent_SetOffset(Uint64 entityID, Vector3f* offset)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<Collider3DComponent>(entity);
	*offset = component.Offset;
}

float Saffron_Collider3DComponent_GetDensity(Uint64 entityID)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<Collider3DComponent>(entity);
	return component.Density;
}

void Saffron_Collider3DComponent_SetDensity(Uint64 entityID, float density)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<Collider3DComponent>(entity);
	component.Density = density;
}

float Saffron_Collider3DComponent_GetFriction(Uint64 entityID)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<Collider3DComponent>(entity);
	return component.Friction;
}

void Saffron_Collider3DComponent_SetFriction(Uint64 entityID, float friction)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<Collider3DComponent>(entity);
	component.Friction = friction;
}

void Saffron_BoxCollider3DComponent_GetSize(Uint64 entityID, Vector3f* size)
{
	SE_CORE_ASSERT(size);
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<BoxCollider3DComponent>(entity);
	*size = component.Size;
}

void Saffron_BoxCollider3DComponent_SetSize(Uint64 entityID, Vector3f* size)
{
	SE_CORE_ASSERT(size);
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<BoxCollider3DComponent>(entity);
	component.Size = *size;
}

float Saffron_SphereCollider3DComponent_GetRadius(Uint64 entityID)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<SphereCollider3DComponent>(entity);
	return component.Radius;
}

void Saffron_SphereCollider3DComponent_SetRadius(Uint64 entityID, float radius)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<SphereCollider3DComponent>(entity);
	component.Radius = radius;
}

Shared<Mesh>* Saffron_Mesh_Constructor(MonoString* filepath)
{
	auto* result = new Mesh(mono_string_to_utf8(filepath));
	return new Shared<Mesh>(result);
}

void Saffron_Mesh_Destructor(Shared<Mesh>* mesh)
{
	delete mesh;
}

Shared<Material>* Saffron_Mesh_GetMaterial(Shared<Mesh>* mesh)
{
	return new Shared<Material>((*mesh)->GetMaterial());
}

Shared<MaterialInstance>* Saffron_Mesh_GetMaterialByIndex(Shared<Mesh>* mesh, int index)
{
	const auto& materials = (*mesh)->GetMaterials();
	SE_CORE_ASSERT(index < materials.size());
	return new Shared<MaterialInstance>(materials[index]);
}

size_t Saffron_Mesh_GetMaterialCount(Shared<Mesh>* mesh)
{
	const auto& materials = (*mesh)->GetMaterials();
	return materials.size();
}

Shared<Texture2D>* Saffron_Texture2D_Constructor(Uint32 width, Uint32 height)
{
	const auto result = Texture2D::Create(TextureFormat::RGBA, width, height);
	return new Shared<Texture2D>(result);
}

void Saffron_Texture2D_Destructor(Shared<Texture2D>* texture)
{
	delete texture;
}

void Saffron_Texture2D_SetData(Shared<Texture2D>* texture, MonoArray* data, Int32 count)
{
	SE_ASSERT(data);

	Shared<Texture2D>& instance = *texture;

	instance->Lock();
	Buffer buffer = instance->GetWriteableBuffer();
	const Uint32 dataSize = count * sizeof(Vector4f) / 4;
	SE_CORE_ASSERT(dataSize <= buffer.Size());
	// Convert RGBA32F color to RGBA8
	auto* pixels = static_cast<Uint8*>(buffer.Data());
	for (Uint32 i = 0; i < instance->GetWidth() * instance->GetHeight(); i++)
	{
		Vector4f& value = mono_array_get(data, Vector4f, i);
		*pixels++ = static_cast<Uint32>(value.x * 255.0f);
		*pixels++ = static_cast<Uint32>(value.y * 255.0f);
		*pixels++ = static_cast<Uint32>(value.z * 255.0f);
		*pixels++ = static_cast<Uint32>(value.w * 255.0f);
	}
	instance->Unlock();
}

void Saffron_Material_Destructor(Shared<Material>* material)
{
	delete material;
}

void Saffron_Material_SetFloat(Shared<Material>* material, MonoString* uniform, float value)
{
	SE_ASSERT(uniform);
	Shared<Material>& instance = *static_cast<Shared<Material>*>(material);
	instance->Set(mono_string_to_utf8(uniform), value);
}

void Saffron_Material_SetTexture(Shared<Material>* material, MonoString* uniform, Shared<Texture2D>* texture)
{
	SE_ASSERT(uniform && texture);
	Shared<Material>& instance = *static_cast<Shared<Material>*>(material);
	instance->Set(mono_string_to_utf8(uniform), *texture);
}

void Saffron_MaterialInstance_Destructor(Shared<MaterialInstance>* instance)
{
	delete instance;
}

void Saffron_MaterialInstance_SetFloat(Shared<MaterialInstance>* instance, MonoString* uniform, float value)
{
	SE_ASSERT(uniform);
	(*instance)->Set(mono_string_to_utf8(uniform), value);
}

void Saffron_MaterialInstance_SetVector3(Shared<MaterialInstance>* instance, MonoString* uniform, Vector3f* value)
{
	SE_ASSERT(uniform && value);
	(*instance)->Set(mono_string_to_utf8(uniform), *value);
}

void Saffron_MaterialInstance_SetVector4(Shared<MaterialInstance>* instance, MonoString* uniform, Vector4f* value)
{
	SE_ASSERT(uniform && value);
	(*instance)->Set(mono_string_to_utf8(uniform), *value);
}

void Saffron_MaterialInstance_SetTexture(Shared<MaterialInstance>* instance, MonoString* uniform,
                                         Shared<Texture2D>* texture)
{
	SE_ASSERT(uniform && texture);
	(*instance)->Set(mono_string_to_utf8(uniform), *texture);
}

Shared<Mesh>* Saffron_MeshFactory_CreatePlane(float width, float height)
{
	// TODO: Implement properly with MeshFactory class!
	return new Shared<Mesh>(new Mesh("Plane1m.obj"));
}

Shared<SceneCamera>* Saffron_Camera_Constructor(Uint32 width, Uint32 height)
{
	return new Shared<SceneCamera>(new SceneCamera(width, height));
}

void Saffron_Camera_Destructor(Shared<SceneCamera>* camera)
{
	delete camera;
}

Uint32 Saffron_Camera_GetProjectionMode(Shared<SceneCamera>* camera)
{
	return static_cast<Uint32>((*camera)->GetProjectionMode());
}

void Saffron_Camera_SetProjectionMode(Shared<SceneCamera>* camera, Uint32 mode)
{
	(*camera)->SetProjectionMode(static_cast<SceneCamera::ProjectionMode>(mode));
}

void* Saffron_CameraComponent_GetCamera(Uint64 entityID)
{
	Entity entity = GetEntityFromActiveScene(entityID);
	auto& cameraComponent = entity.GetComponent<CameraComponent>();
	return new Shared<SceneCamera>(cameraComponent.Camera);
}

void Saffron_CameraComponent_SetCamera(Uint64 entityID, Shared<SceneCamera>* camera)
{
	SE_ASSERT(camera);
	Entity entity = GetEntityFromActiveScene(entityID);
	auto& cameraComponent = entity.GetComponent<CameraComponent>();
	cameraComponent.Camera = camera ? *camera : nullptr;
}

MonoString* Saffron_ScriptComponent_GetModuleName(Uint64 entityID)
{
	Entity entity = GetEntityFromActiveScene(entityID);
	auto& scriptComponent = entity.GetComponent<ScriptComponent>();
	return ScriptEngine::CreateMonoString(scriptComponent.ModuleName.c_str());
}

void Saffron_ScriptComponent_SetModuleName(Uint64 entityID, MonoString* moduleName)
{
	SE_ASSERT(moduleName);
	Entity entity = GetEntityFromActiveScene(entityID);
	auto& scriptComponent = entity.GetComponent<ScriptComponent>();
	scriptComponent.ModuleName = mono_string_to_utf8(moduleName);
}
}
}
