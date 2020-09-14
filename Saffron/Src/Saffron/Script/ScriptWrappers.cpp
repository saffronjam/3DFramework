#include "Saffron/SaffronPCH.h"

#include <Box2D/Box2D.h>
#include <mono/jit/jit.h>

#include "Saffron/Scene/Entity.h"
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
// Input
////////////////////////////////////////////////////////////////

bool Saffron_Input_IsKeyPressed(const Keyboard &keyboard, KeyCode key)
{
	return keyboard.IsPressed(key);
}


////////////////////////////////////////////////////////////////
// Entity //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

void Saffron_Entity_GetTransform(uint64_t entityID, glm::mat4 *outTransform)
{
	Ref<Scene> scene = ScriptEngine::GetCurrentSceneContext();
	SE_CORE_ASSERT(scene, "No active scene!");
	const auto &entityMap = scene->GetEntityMap();
	SE_CORE_ASSERT(entityMap.find(entityID) != entityMap.end(), "Invalid entity ID or entity doesn't exist in scene!");

	Entity entity = entityMap.at(entityID);
	auto &transformComponent = entity.GetComponent<TransformComponent>();
	memcpy(outTransform, glm::value_ptr(transformComponent.Transform), sizeof(glm::mat4));
}

void Saffron_Entity_SetTransform(uint64_t entityID, glm::mat4 *inTransform)
{
	Ref<Scene> scene = ScriptEngine::GetCurrentSceneContext();
	SE_CORE_ASSERT(scene, "No active scene!");
	const auto &entityMap = scene->GetEntityMap();
	SE_CORE_ASSERT(entityMap.find(entityID) != entityMap.end(), "Invalid entity ID or entity doesn't exist in scene!");

	Entity entity = entityMap.at(entityID);
	auto &transformComponent = entity.GetComponent<TransformComponent>();
	memcpy(glm::value_ptr(transformComponent.Transform), inTransform, sizeof(glm::mat4));
}

void Saffron_Entity_CreateComponent(uint64_t entityID, void *type)
{
	Ref<Scene> scene = ScriptEngine::GetCurrentSceneContext();
	SE_CORE_ASSERT(scene, "No active scene!");
	const auto &entityMap = scene->GetEntityMap();
	SE_CORE_ASSERT(entityMap.find(entityID) != entityMap.end(), "Invalid entity ID or entity doesn't exist in scene!");

	Entity entity = entityMap.at(entityID);
	MonoType *monoType = mono_reflection_type_get_type(static_cast<MonoReflectionType *>(type));
	s_CreateComponentFuncs[monoType](entity);
}

bool Saffron_Entity_HasComponent(uint64_t entityID, void *type)
{
	Ref<Scene> scene = ScriptEngine::GetCurrentSceneContext();
	SE_CORE_ASSERT(scene, "No active scene!");
	const auto &entityMap = scene->GetEntityMap();
	SE_CORE_ASSERT(entityMap.find(entityID) != entityMap.end(), "Invalid entity ID or entity doesn't exist in scene!");

	Entity entity = entityMap.at(entityID);
	MonoType *monoType = mono_reflection_type_get_type(static_cast<MonoReflectionType *>(type));
	const bool result = s_HasComponentFuncs[monoType](entity);
	return result;
}

uint64_t Saffron_Entity_FindEntityByTag(MonoString *tag)
{
	Ref<Scene> scene = ScriptEngine::GetCurrentSceneContext();
	SE_CORE_ASSERT(scene, "No active scene!");

	Entity entity = scene->FindEntityByTag(mono_string_to_utf8(tag));
	if ( entity )
		return entity.GetComponent<IDComponent>().ID;

	return 0;
}

void *Saffron_MeshComponent_GetMesh(uint64_t entityID)
{
	Ref<Scene> scene = ScriptEngine::GetCurrentSceneContext();
	SE_CORE_ASSERT(scene, "No active scene!");
	const auto &entityMap = scene->GetEntityMap();
	SE_CORE_ASSERT(entityMap.find(entityID) != entityMap.end(), "Invalid entity ID or entity doesn't exist in scene!");

	Entity entity = entityMap.at(entityID);
	auto &meshComponent = entity.GetComponent<MeshComponent>();
	return new Ref<Mesh>(meshComponent.Mesh);
}

void Saffron_MeshComponent_SetMesh(uint64_t entityID, Ref<Mesh> *inMesh)
{
	Ref<Scene> scene = ScriptEngine::GetCurrentSceneContext();
	SE_CORE_ASSERT(scene, "No active scene!");
	const auto &entityMap = scene->GetEntityMap();
	SE_CORE_ASSERT(entityMap.find(entityID) != entityMap.end(), "Invalid entity ID or entity doesn't exist in scene!");

	Entity entity = entityMap.at(entityID);
	auto &meshComponent = entity.GetComponent<MeshComponent>();
	meshComponent.Mesh = inMesh ? *inMesh : nullptr;
}

void Saffron_RigidBody2DComponent_ApplyLinearImpulse(uint64_t entityID, glm::vec2 *impulse, glm::vec2 *offset, bool wake)
{
	Ref<Scene> scene = ScriptEngine::GetCurrentSceneContext();
	SE_CORE_ASSERT(scene, "No active scene!");
	const auto &entityMap = scene->GetEntityMap();
	SE_CORE_ASSERT(entityMap.find(entityID) != entityMap.end(), "Invalid entity ID or entity doesn't exist in scene!");

	Entity entity = entityMap.at(entityID);
	SE_CORE_ASSERT(entity.HasComponent<RigidBody2DComponent>());
	auto &component = entity.GetComponent<RigidBody2DComponent>();
	auto *body = static_cast<b2Body *>(component.RuntimeBody);
	body->ApplyLinearImpulse(*reinterpret_cast<const b2Vec2 *>(impulse), body->GetWorldCenter() + *reinterpret_cast<const b2Vec2 *>(offset), wake);
}

void Saffron_RigidBody2DComponent_GetLinearVelocity(uint64_t entityID, glm::vec2 *outVelocity)
{
	Ref<Scene> scene = ScriptEngine::GetCurrentSceneContext();
	SE_CORE_ASSERT(scene, "No active scene!");
	const auto &entityMap = scene->GetEntityMap();
	SE_CORE_ASSERT(entityMap.find(entityID) != entityMap.end(), "Invalid entity ID or entity doesn't exist in scene!");

	Entity entity = entityMap.at(entityID);
	SE_CORE_ASSERT(entity.HasComponent<RigidBody2DComponent>());
	auto &component = entity.GetComponent<RigidBody2DComponent>();
	auto *body = static_cast<b2Body *>(component.RuntimeBody);
	const auto &velocity = body->GetLinearVelocity();
	SE_CORE_ASSERT(outVelocity);
	*outVelocity = { velocity.x, velocity.y };
}

void Saffron_RigidBody2DComponent_SetLinearVelocity(uint64_t entityID, glm::vec2 *velocity)
{
	Ref<Scene> scene = ScriptEngine::GetCurrentSceneContext();
	SE_CORE_ASSERT(scene, "No active scene!");
	const auto &entityMap = scene->GetEntityMap();
	SE_CORE_ASSERT(entityMap.find(entityID) != entityMap.end(), "Invalid entity ID or entity doesn't exist in scene!");

	Entity entity = entityMap.at(entityID);
	SE_CORE_ASSERT(entity.HasComponent<RigidBody2DComponent>());
	auto &component = entity.GetComponent<RigidBody2DComponent>();
	auto *body = static_cast<b2Body *>(component.RuntimeBody);
	SE_CORE_ASSERT(velocity);
	body->SetLinearVelocity({ velocity->x, velocity->y });
}

Ref<Mesh> *Saffron_Mesh_Constructor(MonoString *filepath)
{
	return new Ref<Mesh>(new Mesh(mono_string_to_utf8(filepath)));
}

void Saffron_Mesh_Destructor(Ref<Mesh> *_this)
{
	delete _this;
}

Ref<Material> *Saffron_Mesh_GetMaterial(Ref<Mesh> *inMesh)
{
	Ref<Mesh> &mesh = *static_cast<Ref<Mesh> *>(inMesh);
	return new Ref<Material>(mesh->GetMaterial());
}

Ref<MaterialInstance> *Saffron_Mesh_GetMaterialByIndex(Ref<Mesh> *inMesh, int index)
{
	Ref<Mesh> &mesh = *static_cast<Ref<Mesh> *>(inMesh);
	const auto &materials = mesh->GetMaterials();

	SE_CORE_ASSERT(index < materials.size());
	return new Ref<MaterialInstance>(materials[index]);
}

int Saffron_Mesh_GetMaterialCount(Ref<Mesh> *inMesh)
{
	Ref<Mesh> &mesh = *static_cast<Ref<Mesh> *>(inMesh);
	const auto &materials = mesh->GetMaterials();
	return materials.size();
}

void *Saffron_Texture2D_Constructor(Uint32 width, Uint32 height)
{
	const auto result = Texture2D::Create(Texture::Format::RGBA, width, height);
	return new Ref<Texture2D>(result);
}

void Saffron_Texture2D_Destructor(Ref<Texture2D> *_this)
{
	delete _this;
}

void Saffron_Texture2D_SetData(Ref<Texture2D> *_this, MonoArray *inData, Int32 count)
{
	Ref<Texture2D> &instance = *_this;

	const Uint32 dataSize = count * sizeof(glm::vec4) / 4;

	instance->Lock();
	Buffer buffer = instance->GetWriteableBuffer();
	SE_CORE_ASSERT(dataSize <= buffer.Size());
	// Convert RGBA32F color to RGBA8
	auto *pixels = static_cast<Uint8 *>(buffer.Data());
	for ( int i = 0; i < instance->GetWidth() * instance->GetHeight(); i++ )
	{
		glm::vec4 &value = mono_array_get(inData, glm::vec4, i);
		*pixels++ = static_cast<Uint32>(value.x * 255.0f);
		*pixels++ = static_cast<Uint32>(value.y * 255.0f);
		*pixels++ = static_cast<Uint32>(value.z * 255.0f);
		*pixels++ = static_cast<Uint32>(value.w * 255.0f);
	}

	instance->Unlock();
}

void Saffron_Material_Destructor(Ref<Material> *_this)
{
	delete _this;
}

void Saffron_Material_SetFloat(Ref<Material> *_this, MonoString *uniform, float value)
{
	Ref<Material> &instance = *static_cast<Ref<Material> *>(_this);
	instance->Set(mono_string_to_utf8(uniform), value);
}

void Saffron_Material_SetTexture(Ref<Material> *_this, MonoString *uniform, Ref<Texture2D> *texture)
{
	Ref<Material> &instance = *static_cast<Ref<Material> *>(_this);
	instance->Set(mono_string_to_utf8(uniform), *texture);
}

void Saffron_MaterialInstance_Destructor(Ref<MaterialInstance> *_this)
{
	delete _this;
}

void Saffron_MaterialInstance_SetFloat(Ref<MaterialInstance> *_this, MonoString *uniform, float value)
{
	Ref<MaterialInstance> &instance = *static_cast<Ref<MaterialInstance> *>(_this);
	instance->Set(mono_string_to_utf8(uniform), value);
}

void Saffron_MaterialInstance_SetVector3(Ref<MaterialInstance> *_this, MonoString *uniform, glm::vec3 *value)
{
	Ref<MaterialInstance> &instance = *static_cast<Ref<MaterialInstance> *>(_this);
	instance->Set(mono_string_to_utf8(uniform), *value);
}

void Saffron_MaterialInstance_SetVector4(Ref<MaterialInstance> *_this, MonoString *uniform, glm::vec4 *value)
{
	Ref<MaterialInstance> &instance = *static_cast<Ref<MaterialInstance> *>(_this);
	instance->Set(mono_string_to_utf8(uniform), *value);
}

void Saffron_MaterialInstance_SetTexture(Ref<MaterialInstance> *_this, MonoString *uniform, Ref<Texture2D> *texture)
{
	Ref<MaterialInstance> &instance = *static_cast<Ref<MaterialInstance> *>(_this);
	instance->Set(mono_string_to_utf8(uniform), *texture);
}

void *Saffron_MeshFactory_CreatePlane(float width, float height)
{
	// TODO: Implement properly with MeshFactory class!
	return new Ref<Mesh>(new Mesh("assets/models/Plane1m.obj"));
}
}
}