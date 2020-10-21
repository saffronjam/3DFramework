#include "SaffronPCH.h"

#include <box2d/box2d.h>
#include <mono/jit/jit.h>

#include "Saffron/Entity/Entity.h"
#include "Saffron/Script/ScriptWrappers.h"

namespace Se
{

///////////////////////////////////////////////////////////////////////////
/// Externs
///////////////////////////////////////////////////////////////////////////

extern std::unordered_map<MonoType *, std::function<bool(Entity &)>> s_HasComponentFuncs;
extern std::unordered_map<MonoType *, std::function<void(Entity &)>> s_CreateComponentFuncs;

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
	Ref<Scene> scene = ScriptEngine::GetCurrentSceneContext();
	SE_CORE_ASSERT(scene, "No active scene!");
	const auto &entityMap = scene->GetEntityMap();
	SE_CORE_ASSERT(entityMap.find(entityID) != entityMap.end(), "Invalid entity ID or entity doesn't exist in scene!");

	return entityMap.at(entityID);
}

template<typename T>
T &GetComponentWithCheck(Entity entity)
{
	SE_CORE_ASSERT(entity.HasComponent<T>());
	return entity.GetComponent<T>();
}

Ref<Scene> GetSceneWithCheck()
{
	Ref<Scene> scene = ScriptEngine::GetCurrentSceneContext();
	SE_CORE_ASSERT(scene, "No active scene!");
	return scene;
}


////////////////////////////////////////////////////////////////
// Input
////////////////////////////////////////////////////////////////

bool Saffron_Input_IsKeyPressed(KeyCode key)
{
	return Input::IsKeyPressed(key);
}

bool Saffron_Input_IsMouseButtonPressed(MouseButtonCode mouseButton)
{
	return Input::IsMouseButtonPressed(mouseButton);
}


////////////////////////////////////////////////////////////////
// Entity //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

void Saffron_Entity_GetTransform(Uint64 entityID, glm::mat4 *outTransform)
{
	Entity entity = GetEntityFromActiveScene(entityID);
	auto &transformComponent = entity.GetComponent<TransformComponent>();
	memcpy(outTransform, glm::value_ptr(transformComponent.Transform), sizeof(glm::mat4));
}

void Saffron_Entity_SetTransform(Uint64 entityID, glm::mat4 *inTransform)
{
	Entity entity = GetEntityFromActiveScene(entityID);
	auto &transformComponent = entity.GetComponent<TransformComponent>();
	memcpy(glm::value_ptr(transformComponent.Transform), inTransform, sizeof(glm::mat4));
}

void Saffron_Entity_CreateComponent(Uint64 entityID, void *type)
{
	Entity entity = GetEntityFromActiveScene(entityID);
	MonoType *monoType = mono_reflection_type_get_type(static_cast<MonoReflectionType *>(type));
	s_CreateComponentFuncs[monoType](entity);
}

bool Saffron_Entity_HasComponent(Uint64 entityID, void *type)
{
	Entity entity = GetEntityFromActiveScene(entityID);
	MonoType *monoType = mono_reflection_type_get_type(static_cast<MonoReflectionType *>(type));
	const bool result = s_HasComponentFuncs[monoType](entity);
	return result;
}

Uint64 Saffron_Entity_FindEntityByTag(MonoString *tag)
{
	Ref<Scene> scene = GetSceneWithCheck();
	Entity entity = scene->FindEntityByTag(mono_string_to_utf8(tag));
	return entity ? entity.GetComponent<IDComponent>().ID : 0;
}

void *Saffron_MeshComponent_GetMesh(Uint64 entityID)
{
	Entity entity = GetEntityFromActiveScene(entityID);
	auto &meshComponent = entity.GetComponent<MeshComponent>();
	return new Ref<Mesh>(meshComponent.Mesh);
}

void Saffron_MeshComponent_SetMesh(Uint64 entityID, Ref<Mesh> *inMesh)
{
	Entity entity = GetEntityFromActiveScene(entityID);
	auto &meshComponent = entity.GetComponent<MeshComponent>();
	meshComponent.Mesh = inMesh ? *inMesh : nullptr;
}

void Saffron_RigidBody2DComponent_ApplyLinearImpulse(Uint64 entityID, glm::vec2 *impulse, glm::vec2 *offset, bool wake)
{
	SE_CORE_ASSERT(impulse && offset);
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto &component = GetComponentWithCheck<RigidBody2DComponent>(entity);
	auto *body = static_cast<b2Body *>(component.RuntimeBody);
	body->ApplyLinearImpulse(*reinterpret_cast<const b2Vec2 *>(impulse), body->GetWorldCenter() + *reinterpret_cast<const b2Vec2 *>(offset), wake);
}

void Saffron_RigidBody2DComponent_GetLinearVelocity(Uint64 entityID, glm::vec2 *outVelocity)
{
	SE_CORE_ASSERT(outVelocity);
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto &component = GetComponentWithCheck<RigidBody2DComponent>(entity);
	auto *body = static_cast<b2Body *>(component.RuntimeBody);
	const auto &velocity = body->GetLinearVelocity();
	*outVelocity = { velocity.x, velocity.y };
}

void Saffron_RigidBody2DComponent_SetLinearVelocity(Uint64 entityID, glm::vec2 *velocity)
{
	SE_CORE_ASSERT(velocity);
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto &component = GetComponentWithCheck<RigidBody2DComponent>(entity);
	auto *body = static_cast<b2Body *>(component.RuntimeBody);
	body->SetLinearVelocity({ velocity->x, velocity->y });
}

void Saffron_Collider2DComponent_GetOffset(Uint64 entityID, glm::vec2 *offset)
{
	SE_CORE_ASSERT(offset);
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto &component = GetComponentWithCheck<Collider2DComponent>(entity);
	*offset = component.Offset;
}

void Saffron_Collider2DComponent_SetOffset(Uint64 entityID, glm::vec2 *offset)
{
	SE_CORE_ASSERT(offset);
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto &component = GetComponentWithCheck<Collider2DComponent>(entity);
	component.Offset = *offset;
}

float Saffron_Collider2DComponent_GetDensity(Uint64 entityID)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto &component = GetComponentWithCheck<Collider2DComponent>(entity);
	return component.Density;
}

void Saffron_Collider2DComponent_SetDensity(Uint64 entityID, float density)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto &component = GetComponentWithCheck<Collider2DComponent>(entity);
	component.Density = density;
}

float Saffron_Collider2DComponent_GetFriction(Uint64 entityID)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto &component = GetComponentWithCheck<Collider2DComponent>(entity);
	return component.Friction;
}

void Saffron_Collider2DComponent_SetFriction(Uint64 entityID, float friction)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto &component = GetComponentWithCheck<Collider2DComponent>(entity);
	component.Friction = friction;
}

void Saffron_BoxCollider2DComponent_GetSize(Uint64 entityID, glm::vec2 *size)
{
	SE_CORE_ASSERT(size);
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto &component = GetComponentWithCheck<BoxCollider2DComponent>(entity);
	*size = component.Size;
}

void Saffron_BoxCollider2DComponent_SetSize(Uint64 entityID, glm::vec2 *size)
{
	SE_CORE_ASSERT(size);
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto &component = GetComponentWithCheck<BoxCollider2DComponent>(entity);
	component.Size = *size;
}

float Saffron_CircleCollider2DComponent_GetRadius(Uint64 entityID)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto &component = GetComponentWithCheck<CircleCollider2DComponent>(entity);
	return component.Radius;
}

void Saffron_CircleCollider2DComponent_SetRadius(Uint64 entityID, float radius)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto &component = GetComponentWithCheck<CircleCollider2DComponent>(entity);
	component.Radius = radius;
}

Ref<Mesh> *Saffron_Mesh_Constructor(MonoString *filepath)
{
	auto *result = new Mesh(mono_string_to_utf8(filepath));
	return new Ref<Mesh>(result);
}

void Saffron_Mesh_Destructor(Ref<Mesh> *mesh)
{
	delete mesh;
}

Ref<Material> *Saffron_Mesh_GetMaterial(Ref<Mesh> *mesh)
{
	return new Ref<Material>((*mesh)->GetMaterial());
}

Ref<MaterialInstance> *Saffron_Mesh_GetMaterialByIndex(Ref<Mesh> *mesh, int index)
{
	const auto &materials = (*mesh)->GetMaterials();
	SE_CORE_ASSERT(index < materials.size());
	return new Ref<MaterialInstance>(materials[index]);
}

int Saffron_Mesh_GetMaterialCount(Ref<Mesh> *mesh)
{
	const auto &materials = (*mesh)->GetMaterials();
	return materials.size();
}

Ref<Texture2D> *Saffron_Texture2D_Constructor(Uint32 width, Uint32 height)
{
	const auto result = Texture2D::Create(Texture::Format::RGBA, width, height);
	return new Ref<Texture2D>(result);
}

void Saffron_Texture2D_Destructor(Ref<Texture2D> *texture)
{
	delete texture;
}

void Saffron_Texture2D_SetData(Ref<Texture2D> *texture, MonoArray *data, Int32 count)
{
	SE_ASSERT(data);

	Ref<Texture2D> &instance = *texture;

	instance->Lock();
	Buffer buffer = instance->GetWriteableBuffer();
	const Uint32 dataSize = count * sizeof(glm::vec4) / 4;
	SE_CORE_ASSERT(dataSize <= buffer.Size());
	// Convert RGBA32F color to RGBA8
	auto *pixels = static_cast<Uint8 *>(buffer.Data());
	for ( int i = 0; i < instance->GetWidth() * instance->GetHeight(); i++ )
	{
		glm::vec4 &value = mono_array_get(data, glm::vec4, i);
		*pixels++ = static_cast<Uint32>(value.x * 255.0f);
		*pixels++ = static_cast<Uint32>(value.y * 255.0f);
		*pixels++ = static_cast<Uint32>(value.z * 255.0f);
		*pixels++ = static_cast<Uint32>(value.w * 255.0f);
	}
	instance->Unlock();
}

void Saffron_Material_Destructor(Ref<Material> *material)
{
	delete material;
}

void Saffron_Material_SetFloat(Ref<Material> *material, MonoString *uniform, float value)
{
	SE_ASSERT(uniform);
	Ref<Material> &instance = *static_cast<Ref<Material> *>(material);
	instance->Set(mono_string_to_utf8(uniform), value);
}

void Saffron_Material_SetTexture(Ref<Material> *material, MonoString *uniform, Ref<Texture2D> *texture)
{
	SE_ASSERT(uniform && texture);
	Ref<Material> &instance = *static_cast<Ref<Material> *>(material);
	instance->Set(mono_string_to_utf8(uniform), *texture);
}

void Saffron_MaterialInstance_Destructor(Ref<MaterialInstance> *instance)
{
	delete instance;
}

void Saffron_MaterialInstance_SetFloat(Ref<MaterialInstance> *instance, MonoString *uniform, float value)
{
	SE_ASSERT(uniform);
	(*instance)->Set(mono_string_to_utf8(uniform), value);
}

void Saffron_MaterialInstance_SetVector3(Ref<MaterialInstance> *instance, MonoString *uniform, glm::vec3 *value)
{
	SE_ASSERT(uniform && value);
	(*instance)->Set(mono_string_to_utf8(uniform), *value);
}

void Saffron_MaterialInstance_SetVector4(Ref<MaterialInstance> *instance, MonoString *uniform, glm::vec4 *value)
{
	SE_ASSERT(uniform && value);
	(*instance)->Set(mono_string_to_utf8(uniform), *value);
}

void Saffron_MaterialInstance_SetTexture(Ref<MaterialInstance> *instance, MonoString *uniform, Ref<Texture2D> *texture)
{
	SE_ASSERT(uniform && texture);
	(*instance)->Set(mono_string_to_utf8(uniform), *texture);
}

Ref<Mesh> *Saffron_MeshFactory_CreatePlane(float width, float height)
{
	// TODO: Implement properly with MeshFactory class!
	return new Ref<Mesh>(new Mesh("Assets/models/Plane1m.obj"));
}

Ref<SceneCamera> *Saffron_Camera_Constructor(Uint32 width, Uint32 height)
{
	return new Ref<SceneCamera>(new SceneCamera(width, height));
}

void Saffron_Camera_Destructor(Ref<SceneCamera> *camera)
{
	delete camera;
}

Uint32 Saffron_Camera_GetProjectionMode(Ref<SceneCamera> *camera)
{
	return static_cast<Uint32>((*camera)->GetProjectionMode());
}

void Saffron_Camera_SetProjectionMode(Ref<SceneCamera> *camera, Uint32 mode)
{
	(*camera)->SetProjectionMode(static_cast<SceneCamera::ProjectionMode>(mode));
}

void *Saffron_CameraComponent_GetCamera(Uint64 entityID)
{
	Entity entity = GetEntityFromActiveScene(entityID);
	auto &cameraComponent = entity.GetComponent<CameraComponent>();
	return new Ref<SceneCamera>(cameraComponent.Camera);
}

void Saffron_CameraComponent_SetCamera(Uint64 entityID, Ref<SceneCamera> *camera)
{
	SE_ASSERT(camera);
	Entity entity = GetEntityFromActiveScene(entityID);
	auto &cameraComponent = entity.GetComponent<CameraComponent>();
	cameraComponent.Camera = camera ? *camera : nullptr;
}

MonoString *Saffron_ScriptComponent_GetModuleName(Uint64 entityID)
{
	Entity entity = GetEntityFromActiveScene(entityID);
	auto &scriptComponent = entity.GetComponent<ScriptComponent>();
	return ScriptEngine::CreateMonoString(scriptComponent.ModuleName.c_str());
}

void Saffron_ScriptComponent_SetModuleName(Uint64 entityID, MonoString *moduleName)
{
	SE_ASSERT(moduleName);
	Entity entity = GetEntityFromActiveScene(entityID);
	auto &scriptComponent = entity.GetComponent<ScriptComponent>();
	scriptComponent.ModuleName = mono_string_to_utf8(moduleName);
}
}
}