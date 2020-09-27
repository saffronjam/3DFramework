#pragma once

#include "Saffron/Input/KeyCodes.h"
#include "Saffron/Renderer/Mesh.h"
#include "Saffron/Renderer/Material.h"
#include "Saffron/Script/ScriptEngine.h"

extern "C" {
	typedef struct _MonoString MonoString;
	typedef struct _MonoArray MonoArray;
}


namespace Se
{
namespace Script
{
// Input
bool Saffron_Input_IsKeyPressed(KeyCode key);

// Entity
void Saffron_Entity_GetTransform(Uint64 entityID, glm::mat4 *outTransform);
void Saffron_Entity_SetTransform(Uint64 entityID, glm::mat4 *inTransform);
void Saffron_Entity_CreateComponent(Uint64 entityID, void *type);
bool Saffron_Entity_HasComponent(Uint64 entityID, void *type);
Uint64 Saffron_Entity_FindEntityByTag(MonoString *tag);

void *Saffron_MeshComponent_GetMesh(Uint64 entityID);
void Saffron_MeshComponent_SetMesh(Uint64 entityID, Ref<Mesh> *inMesh);

void Saffron_RigidBody2DComponent_ApplyLinearImpulse(Uint64 entityID, glm::vec2 *impulse, glm::vec2 *offset, bool wake);
void Saffron_RigidBody2DComponent_GetLinearVelocity(Uint64 entityID, glm::vec2 *outVelocity);
void Saffron_RigidBody2DComponent_SetLinearVelocity(Uint64 entityID, glm::vec2 *velocity);

// Renderer
// Texture2D
void *Saffron_Texture2D_Constructor(Uint32 width, Uint32 height);
void Saffron_Texture2D_Destructor(Ref<Texture2D> *_this);
void Saffron_Texture2D_SetData(Ref<Texture2D> *_this, MonoArray *inData, Int32 count);

// Material
void Saffron_Material_Destructor(Ref<Material> *_this);
void Saffron_Material_SetFloat(Ref<Material> *_this, MonoString *uniform, float value);
void Saffron_Material_SetTexture(Ref<Material> *_this, MonoString *uniform, Ref<Texture2D> *texture);

void Saffron_MaterialInstance_Destructor(Ref<MaterialInstance> *_this);
void Saffron_MaterialInstance_SetFloat(Ref<MaterialInstance> *_this, MonoString *uniform, float value);
void Saffron_MaterialInstance_SetVector3(Ref<MaterialInstance> *_this, MonoString *uniform, glm::vec3 *value);
void Saffron_MaterialInstance_SetVector4(Ref<MaterialInstance> *_this, MonoString *uniform, glm::vec4 *value);
void Saffron_MaterialInstance_SetTexture(Ref<MaterialInstance> *_this, MonoString *uniform, Ref<Texture2D> *texture);

// Mesh
Ref<Mesh> *Saffron_Mesh_Constructor(MonoString *filepath);
void Saffron_Mesh_Destructor(Ref<Mesh> *_this);
Ref<Material> *Saffron_Mesh_GetMaterial(Ref<Mesh> *inMesh);
Ref<MaterialInstance> *Saffron_Mesh_GetMaterialByIndex(Ref<Mesh> *inMesh, int index);
int Saffron_Mesh_GetMaterialCount(Ref<Mesh> *inMesh);

void *Saffron_MeshFactory_CreatePlane(float width, float height);

// Scene Camera
Ref<SceneCamera> *Saffron_SceneCamera_Constructor(Uint32 width, Uint32 height);
void Saffron_SceneCamera_Destructor(Ref<SceneCamera> *_this);
// Camera Component
Ref<SceneCamera> *Saffron_CameraComponent_GetCamera(Uint64 entityID);
void Saffron_CameraComponent_SetCamera(Uint64 entityID, Ref<SceneCamera> *inCamera);



}
}

