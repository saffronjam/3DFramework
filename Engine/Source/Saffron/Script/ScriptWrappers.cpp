#include "SaffronPCH.h"

#include <box2d/box2d.h>
#include <mono/jit/jit.h>

#include "Saffron/Input/Keyboard.h"
#include "Saffron/Input/Mouse.h"
#include "Saffron/Entity/Entity.h"
#include "Saffron/Script/ScriptRegistry.h"
#include "Saffron/Script/ScriptWrappers.h"

namespace Se
{
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
	Shared<Scene> scene = ScriptEngine::GetSceneContext();
	Debug::Assert(scene, "No active scene!");;
	const auto& entityMap = scene->GetEntityMap();
	Debug::Assert(entityMap.find(entityID) != entityMap.end(), "Invalid entity ID or entity doesn't exist in scene!");;

	return entityMap.at(entityID);
}

template <typename T>
T& GetComponentWithCheck(Entity entity)
{
	Debug::Assert(entity.HasComponent<T>());;
	return entity.GetComponent<T>();
}

Shared<Scene> GetSceneWithCheck()
{
	Shared<Scene> scene = ScriptEngine::GetSceneContext();
	Debug::Assert(scene, "No active scene!");;
	return scene;
}


////////////////////////////////////////////////////////////////
// Input
////////////////////////////////////////////////////////////////

bool Saffron_Input_IsKeyPressed(KeyCode key)
{
	return Keyboard::IsPressed(key);
}

bool Saffron_Input_IsKeyDown(KeyCode key)
{
	return Keyboard::IsDown(key);
}

bool Saffron_Input_IsMouseButtonPressed(MouseButtonCode mouseButton)
{
	return Mouse::IsPressed(mouseButton);
}


////////////////////////////////////////////////////////////////
// Entity //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

void Saffron_Entity_GetTransform(ulong entityID, Matrix4* outTransform)
{
	Entity entity = GetEntityFromActiveScene(entityID);
	auto& transformComponent = entity.GetComponent<TransformComponent>();
	memcpy(outTransform, value_ptr(transformComponent.Transform), sizeof(Matrix4));
}

void Saffron_Entity_SetTransform(ulong entityID, Matrix4* inTransform)
{
	Entity entity = GetEntityFromActiveScene(entityID);
	auto& transformComponent = entity.GetComponent<TransformComponent>();
	memcpy(value_ptr(transformComponent.Transform), inTransform, sizeof(Matrix4));
}

void Saffron_Entity_CreateComponent(ulong entityID, void* type)
{
	Entity entity = GetEntityFromActiveScene(entityID);
	Mono::Type monoType(static_cast<MonoReflectionType*>(type));
	ScriptRegistry::Instance().CreateComponent(monoType, entity);
}

bool Saffron_Entity_HasComponent(ulong entityID, void* type)
{
	Entity entity = GetEntityFromActiveScene(entityID);
	const Mono::Type monoType(static_cast<MonoReflectionType*>(type));
	const bool result = ScriptRegistry::Instance().HasComponent(monoType, entity);
	return result;
}

ulong Saffron_Entity_GetEntity(MonoString* tag)
{
	Shared<Scene> scene = GetSceneWithCheck();
	Entity entity = scene->GetEntity(mono_string_to_utf8(tag));
	return entity ? entity.GetComponent<IDComponent>().ID : UUID(0);
}

void* Saffron_MeshComponent_GetMesh(ulong entityID)
{
	Entity entity = GetEntityFromActiveScene(entityID);
	auto& meshComponent = entity.GetComponent<MeshComponent>();
	return new Shared<Mesh>(meshComponent.Mesh);
}

void Saffron_MeshComponent_SetMesh(ulong entityID, Shared<Mesh>* inMesh)
{
	Entity entity = GetEntityFromActiveScene(entityID);
	auto& meshComponent = entity.GetComponent<MeshComponent>();
	meshComponent.Mesh = inMesh ? *inMesh : nullptr;
}

void Saffron_RigidBody2DComponent_ApplyLinearImpulse(ulong entityID, Vector2* impulse, Vector2* offset, bool wake)
{
	Debug::Assert(impulse && offset);;
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<RigidBody2DComponent>(entity);
	auto* body = static_cast<b2Body*>(component.RuntimeBody);
	body->ApplyLinearImpulse(*reinterpret_cast<const b2Vec2*>(impulse),
	                         body->GetWorldCenter() + *reinterpret_cast<const b2Vec2*>(offset), wake);
}

void Saffron_RigidBody2DComponent_GetLinearVelocity(ulong entityID, Vector2* outVelocity)
{
	Debug::Assert(outVelocity);;
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<RigidBody2DComponent>(entity);
	auto* body = static_cast<b2Body*>(component.RuntimeBody);
	const auto& velocity = body->GetLinearVelocity();
	*outVelocity = {velocity.x, velocity.y};
}

void Saffron_RigidBody2DComponent_SetLinearVelocity(ulong entityID, Vector2* velocity)
{
	Debug::Assert(velocity);;
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<RigidBody2DComponent>(entity);
	auto* body = static_cast<b2Body*>(component.RuntimeBody);
	body->SetLinearVelocity({velocity->x, velocity->y});
}

void Saffron_Collider2DComponent_GetOffset(ulong entityID, Vector2* offset)
{
	Debug::Assert(offset);;
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<Collider2DComponent>(entity);
	*offset = component.Offset;
}

void Saffron_Collider2DComponent_SetOffset(ulong entityID, Vector2* offset)
{
	Debug::Assert(offset);;
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<Collider2DComponent>(entity);
	component.Offset = *offset;
}

float Saffron_Collider2DComponent_GetDensity(ulong entityID)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<Collider2DComponent>(entity);
	return component.Density;
}

void Saffron_Collider2DComponent_SetDensity(ulong entityID, float density)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<Collider2DComponent>(entity);
	component.Density = density;
}

float Saffron_Collider2DComponent_GetFriction(ulong entityID)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<Collider2DComponent>(entity);
	return component.Friction;
}

void Saffron_Collider2DComponent_SetFriction(ulong entityID, float friction)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<Collider2DComponent>(entity);
	component.Friction = friction;
}

void Saffron_BoxCollider2DComponent_GetSize(ulong entityID, Vector2* size)
{
	Debug::Assert(size);;
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<BoxCollider2DComponent>(entity);
	*size = component.Size;
}

void Saffron_BoxCollider2DComponent_SetSize(ulong entityID, Vector2* size)
{
	Debug::Assert(size);;
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<BoxCollider2DComponent>(entity);
	component.Size = *size;
}

float Saffron_CircleCollider2DComponent_GetRadius(ulong entityID)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<CircleCollider2DComponent>(entity);
	return component.Radius;
}

void Saffron_CircleCollider2DComponent_SetRadius(ulong entityID, float radius)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<CircleCollider2DComponent>(entity);
	component.Radius = radius;
}

void Saffron_RigidBody3DComponent_ApplyLinearImpulse(ulong entityID, Vector3* impulse, Vector3* offset, bool wake)
{
	Debug::Assert(impulse && offset);;
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<RigidBody3DComponent>(entity);
	//auto* body = static_cast<reactphysics3d::RigidBody*>(component.RuntimeBody);
	//body->applyForceAtLocalPosition(*reinterpret_cast<const reactphysics3d::Vector3*>(impulse),
	//                                body->getLocalCenterOfMass() + *reinterpret_cast<const reactphysics3d::Vector3*>(
	//	                                offset));
}

void Saffron_RigidBody3DComponent_GetLinearVelocity(ulong entityID, Vector3* outVelocity)
{
	Debug::Assert(outVelocity);;
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<RigidBody3DComponent>(entity);
	//auto* body = static_cast<reactphysics3d::RigidBody*>(component.RuntimeBody);
	//const auto& velocity = body->getLinearVelocity();
	//*outVelocity = {velocity.x, velocity.y, velocity.z};
}

void Saffron_RigidBody3DComponent_SetLinearVelocity(ulong entityID, Vector3* velocity)
{
	Debug::Assert(velocity);;
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<RigidBody3DComponent>(entity);
	//auto* body = static_cast<reactphysics3d::RigidBody*>(component.RuntimeBody);
	//body->setLinearVelocity({velocity->x, velocity->y, velocity->z});
}

void Saffron_Collider3DComponent_GetOffset(ulong entityID, Vector3* offset)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<Collider3DComponent>(entity);
	*offset = component.Offset;
}

void Saffron_Collider3DComponent_SetOffset(ulong entityID, Vector3* offset)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<Collider3DComponent>(entity);
	*offset = component.Offset;
}

float Saffron_Collider3DComponent_GetDensity(ulong entityID)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<Collider3DComponent>(entity);
	return component.Density;
}

void Saffron_Collider3DComponent_SetDensity(ulong entityID, float density)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<Collider3DComponent>(entity);
	component.Density = density;
}

float Saffron_Collider3DComponent_GetFriction(ulong entityID)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<Collider3DComponent>(entity);
	return component.Friction;
}

void Saffron_Collider3DComponent_SetFriction(ulong entityID, float friction)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<Collider3DComponent>(entity);
	component.Friction = friction;
}

void Saffron_BoxCollider3DComponent_GetSize(ulong entityID, Vector3* size)
{
	Debug::Assert(size);;
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<BoxCollider3DComponent>(entity);
	*size = component.Size;
}

void Saffron_BoxCollider3DComponent_SetSize(ulong entityID, Vector3* size)
{
	Debug::Assert(size);;
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<BoxCollider3DComponent>(entity);
	component.Size = *size;
}

float Saffron_SphereCollider3DComponent_GetRadius(ulong entityID)
{
	const Entity entity = GetEntityFromActiveScene(entityID);
	auto& component = GetComponentWithCheck<SphereCollider3DComponent>(entity);
	return component.Radius;
}

void Saffron_SphereCollider3DComponent_SetRadius(ulong entityID, float radius)
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
	Debug::Assert(index < materials.size());;
	return new Shared<MaterialInstance>(materials[index]);
}

size_t Saffron_Mesh_GetMaterialCount(Shared<Mesh>* mesh)
{
	const auto& materials = (*mesh)->GetMaterials();
	return materials.size();
}

Shared<Texture2D>* Saffron_Texture2D_Constructor(uint width, uint height)
{
	const auto result = Texture2D::Create(TextureFormat::RGBA, width, height);
	return new Shared<Texture2D>(result);
}

void Saffron_Texture2D_Destructor(Shared<Texture2D>* texture)
{
	delete texture;
}

void Saffron_Texture2D_SetData(Shared<Texture2D>* texture, MonoArray* data, int count)
{
	Debug::Assert(data);

	Shared<Texture2D>& instance = *texture;

	instance->Lock();
	Buffer buffer = instance->GetWriteableBuffer();
	const uint dataSize = count * sizeof(Vector4) / 4;
	Debug::Assert(dataSize <= buffer.Size());;
	// Convert RGBA32F color to RGBA8
	auto* pixels = static_cast<uchar*>(buffer.Data());
	for (uint i = 0; i < instance->GetWidth() * instance->GetHeight(); i++)
	{
		Vector4& value = mono_array_get(data, Vector4, i);
		*pixels++ = static_cast<uint>(value.x * 255.0f);
		*pixels++ = static_cast<uint>(value.y * 255.0f);
		*pixels++ = static_cast<uint>(value.z * 255.0f);
		*pixels++ = static_cast<uint>(value.w * 255.0f);
	}
	instance->Unlock();
}

void Saffron_Material_Destructor(Shared<Material>* material)
{
	delete material;
}

void Saffron_Material_SetFloat(Shared<Material>* material, MonoString* uniform, float value)
{
	Debug::Assert(uniform);
	Shared<Material>& instance = *static_cast<Shared<Material>*>(material);
	instance->Set(mono_string_to_utf8(uniform), value);
}

void Saffron_Material_SetTexture(Shared<Material>* material, MonoString* uniform, Shared<Texture2D>* texture)
{
	Debug::Assert(uniform && texture);
	Shared<Material>& instance = *static_cast<Shared<Material>*>(material);
	instance->Set(mono_string_to_utf8(uniform), *texture);
}

void Saffron_MaterialInstance_Destructor(Shared<MaterialInstance>* instance)
{
	delete instance;
}

void Saffron_MaterialInstance_SetFloat(Shared<MaterialInstance>* instance, MonoString* uniform, float value)
{
	Debug::Assert(uniform);
	(*instance)->Set(mono_string_to_utf8(uniform), value);
}

void Saffron_MaterialInstance_SetVector3(Shared<MaterialInstance>* instance, MonoString* uniform, Vector3* value)
{
	Debug::Assert(uniform && value);
	(*instance)->Set(mono_string_to_utf8(uniform), *value);
}

void Saffron_MaterialInstance_SetVector4(Shared<MaterialInstance>* instance, MonoString* uniform, Vector4* value)
{
	Debug::Assert(uniform && value);
	(*instance)->Set(mono_string_to_utf8(uniform), *value);
}

void Saffron_MaterialInstance_SetTexture(Shared<MaterialInstance>* instance, MonoString* uniform,
                                         Shared<Texture2D>* texture)
{
	Debug::Assert(uniform && texture);
	(*instance)->Set(mono_string_to_utf8(uniform), *texture);
}

Shared<Mesh>* Saffron_MeshFactory_CreatePlane(float width, float height)
{
	// TODO: Implement properly with MeshFactory class!
	return new Shared<Mesh>(new Mesh("Plane1m.obj"));
}

Shared<SceneCamera>* Saffron_Camera_Constructor(uint width, uint height)
{
	return new Shared<SceneCamera>(new SceneCamera(width, height));
}

void Saffron_Camera_Destructor(Shared<SceneCamera>* camera)
{
	delete camera;
}

uint Saffron_Camera_GetProjectionMode(Shared<SceneCamera>* camera)
{
	return static_cast<uint>((*camera)->GetProjectionMode());
}

void Saffron_Camera_SetProjectionMode(Shared<SceneCamera>* camera, uint mode)
{
	(*camera)->SetProjectionMode(static_cast<SceneCamera::ProjectionMode>(mode));
}

void* Saffron_CameraComponent_GetCamera(ulong entityID)
{
	Entity entity = GetEntityFromActiveScene(entityID);
	auto& cameraComponent = entity.GetComponent<CameraComponent>();
	return new Shared<SceneCamera>(cameraComponent.Camera);
}

void Saffron_CameraComponent_SetCamera(ulong entityID, Shared<SceneCamera>* camera)
{
	Debug::Assert(camera);
	Entity entity = GetEntityFromActiveScene(entityID);
	auto& cameraComponent = entity.GetComponent<CameraComponent>();
	cameraComponent.Camera = camera ? *camera : nullptr;
}

MonoString* Saffron_ScriptComponent_GetModuleName(ulong entityID)
{
	Entity entity = GetEntityFromActiveScene(entityID);
	auto& scriptComponent = entity.GetComponent<ScriptComponent>();
	return reinterpret_cast<MonoString*>(Mono::Domain::GetCurrentDomain().CreateString("test").GetInternalPtr());
}

void Saffron_ScriptComponent_SetModuleName(ulong entityID, MonoString* moduleName)
{
	Debug::Assert(moduleName);
	Entity entity = GetEntityFromActiveScene(entityID);
	auto& scriptComponent = entity.GetComponent<ScriptComponent>();
	scriptComponent.Fullname = mono_string_to_utf8(moduleName);
}
}
}
