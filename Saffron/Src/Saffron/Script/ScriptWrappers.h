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

///////////////////////////////////////////////////////////////
/// Input
///////////////////////////////////////////////////////////////

bool Saffron_Input_IsKeyPressed(KeyCode key);
bool Saffron_Input_IsMouseButtonPressed(ButtonCode key);


///////////////////////////////////////////////////////////////
/// Entity
///////////////////////////////////////////////////////////////

void Saffron_Entity_GetTransform(Uint64 entityID, glm::mat4 *outTransform);
void Saffron_Entity_SetTransform(Uint64 entityID, glm::mat4 *inTransform);
void Saffron_Entity_CreateComponent(Uint64 entityID, void *type);
bool Saffron_Entity_HasComponent(Uint64 entityID, void *type);
Uint64 Saffron_Entity_FindEntityByTag(MonoString *tag);


///////////////////////////////////////////////////////////////
/// Texture2D
///////////////////////////////////////////////////////////////

Ref<Texture2D> *Saffron_Texture2D_Constructor(Uint32 width, Uint32 height);
void Saffron_Texture2D_Destructor(Ref<Texture2D> *texture);
void Saffron_Texture2D_SetData(Ref<Texture2D> *texture, MonoArray *data, Int32 count);


///////////////////////////////////////////////////////////////
/// Material
///////////////////////////////////////////////////////////////

void Saffron_Material_Destructor(Ref<Material> *material);
void Saffron_Material_SetFloat(Ref<Material> *material, MonoString *uniform, float value);
void Saffron_Material_SetTexture(Ref<Material> *material, MonoString *uniform, Ref<Texture2D> *texture);


///////////////////////////////////////////////////////////////
/// Material Instance
///////////////////////////////////////////////////////////////

void Saffron_MaterialInstance_Destructor(Ref<MaterialInstance> *instance);
void Saffron_MaterialInstance_SetFloat(Ref<MaterialInstance> *instance, MonoString *uniform, float value);
void Saffron_MaterialInstance_SetVector3(Ref<MaterialInstance> *instance, MonoString *uniform, glm::vec3 *value);
void Saffron_MaterialInstance_SetVector4(Ref<MaterialInstance> *instance, MonoString *uniform, glm::vec4 *value);
void Saffron_MaterialInstance_SetTexture(Ref<MaterialInstance> *instance, MonoString *uniform, Ref<Texture2D> *texture);


///////////////////////////////////////////////////////////////
/// Mesh
///////////////////////////////////////////////////////////////

Ref<Mesh> *Saffron_Mesh_Constructor(MonoString *filepath);
void Saffron_Mesh_Destructor(Ref<Mesh> *mesh);
Ref<Material> *Saffron_Mesh_GetMaterial(Ref<Mesh> *mesh);
Ref<MaterialInstance> *Saffron_Mesh_GetMaterialByIndex(Ref<Mesh> *mesh, int index);
int Saffron_Mesh_GetMaterialCount(Ref<Mesh> *mesh);


///////////////////////////////////////////////////////////////
/// Mesh Factory
///////////////////////////////////////////////////////////////

Ref<Mesh> *Saffron_MeshFactory_CreatePlane(float width, float height);


///////////////////////////////////////////////////////////////
/// Scene Camera
///////////////////////////////////////////////////////////////

Ref<SceneCamera> *Saffron_Camera_Constructor(Uint32 width, Uint32 height);
void Saffron_Camera_Destructor(Ref<SceneCamera> *camera);

Uint32 Saffron_Camera_GetProjectionMode(Ref<SceneCamera> *camera);
void Saffron_Camera_SetProjectionMode(Ref<SceneCamera> *camera, Uint32 mode);

//void Saffron_SceneCamera_SetPerspectiveVerticalFOV(Ref<SceneCamera> *_this, float verticalFov);
//float Saffron_SceneCamera_GetPerspectiveVerticalFOV(Ref<SceneCamera> *_this);
//void Saffron_SceneCamera_SetPerspectiveNearClip(Ref<SceneCamera> *_this, float nearClip);
//float Saffron_SceneCamera_GetPerspectiveNearClip(Ref<SceneCamera> *_this);
//void Saffron_SceneCamera_SetPerspectiveFarClip(Ref<SceneCamera> *_this, float farClip);
//float Saffron_SceneCamera_GetPerspectiveFarClip(Ref<SceneCamera> *_this);
//
//void Saffron_SceneCamera_SetOrthographicSize(Ref<SceneCamera> *_this, float size);
//float Saffron_SceneCamera_GetOrthographicSize(Ref<SceneCamera> *_this);
//void Saffron_SceneCamera_SetOrthographicNearClip(Ref<SceneCamera> *_this, float nearClip);
//float Saffron_SceneCamera_GetOrthographicNearClip(Ref<SceneCamera> *_this);
//void Saffron_SceneCamera_SetOrthographicFarClip(Ref<SceneCamera> *_this, float farClip);
//float Saffron_SceneCamera_GetOrthographicFarClip(Ref<SceneCamera> *_this);


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
/// Components
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
/// Camera Component
///////////////////////////////////////////////////////////////

void *Saffron_CameraComponent_GetCamera(Uint64 entityID);
void Saffron_CameraComponent_SetCamera(Uint64 entityID, Ref<SceneCamera> *inCamera);

///////////////////////////////////////////////////////////////
/// Script Component
///////////////////////////////////////////////////////////////

MonoString *Saffron_ScriptComponent_GetModuleName(Uint64 entityID);
void Saffron_ScriptComponent_SetModuleName(Uint64 entityID, MonoString *moduleName);


///////////////////////////////////////////////////////////////
/// Mesh Component
///////////////////////////////////////////////////////////////

void *Saffron_MeshComponent_GetMesh(Uint64 entityID);
void Saffron_MeshComponent_SetMesh(Uint64 entityID, Ref<Mesh> *inMesh);


///////////////////////////////////////////////////////////////
/// RigidBody2D Component
///////////////////////////////////////////////////////////////

void Saffron_RigidBody2DComponent_ApplyLinearImpulse(Uint64 entityID, glm::vec2 *impulse, glm::vec2 *offset, bool wake);
void Saffron_RigidBody2DComponent_GetLinearVelocity(Uint64 entityID, glm::vec2 *outVelocity);
void Saffron_RigidBody2DComponent_SetLinearVelocity(Uint64 entityID, glm::vec2 *velocity);


///////////////////////////////////////////////////////////////
/// Collider2D Component
///////////////////////////////////////////////////////////////

void Saffron_Collider2DComponent_GetOffset(Uint64 entityID, glm::vec2 *offset);
void Saffron_Collider2DComponent_SetOffset(Uint64 entityID, glm::vec2 *offset);
float Saffron_Collider2DComponent_GetDensity(Uint64 entityID);
void Saffron_Collider2DComponent_SetDensity(Uint64 entityID, float density);
float Saffron_Collider2DComponent_GetFriction(Uint64 entityID);
void Saffron_Collider2DComponent_SetFriction(Uint64 entityID, float friction);


///////////////////////////////////////////////////////////////
/// BoxCollider2D Component
///////////////////////////////////////////////////////////////

void Saffron_BoxCollider2DComponent_GetSize(Uint64 entityID, glm::vec2 *size);
void Saffron_BoxCollider2DComponent_SetSize(Uint64 entityID, glm::vec2 *size);


///////////////////////////////////////////////////////////////
/// CircleCollider2D Component
///////////////////////////////////////////////////////////////

float Saffron_CircleCollider2DComponent_GetRadius(Uint64 entityID);
void Saffron_CircleCollider2DComponent_SetRadius(Uint64 entityID, float radius);

}
}

