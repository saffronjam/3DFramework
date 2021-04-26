#pragma once

#include "Saffron/Input/InputCodes.h"
#include "Saffron/Rendering/Mesh.h"
#include "Saffron/Rendering/Material.h"
#include "Saffron/Script/ScriptEngine.h"

extern "C"
{
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
bool Saffron_Input_IsKeyDown(KeyCode key);
bool Saffron_Input_IsMouseButtonPressed(MouseButtonCode mouseButton);


///////////////////////////////////////////////////////////////
/// Entity
///////////////////////////////////////////////////////////////

void Saffron_Entity_GetTransform(ulong entityID, Matrix4* outTransform);
void Saffron_Entity_SetTransform(ulong entityID, Matrix4* inTransform);
void Saffron_Entity_CreateComponent(ulong entityID, void* type);
bool Saffron_Entity_HasComponent(ulong entityID, void* type);
ulong Saffron_Entity_GetEntity(MonoString* tag);


///////////////////////////////////////////////////////////////
/// Texture2D
///////////////////////////////////////////////////////////////

Shared<Texture2D>* Saffron_Texture2D_Constructor(uint width, uint height);
void Saffron_Texture2D_Destructor(Shared<Texture2D>* texture);
void Saffron_Texture2D_SetData(Shared<Texture2D>* texture, MonoArray* data, int count);


///////////////////////////////////////////////////////////////
/// Material
///////////////////////////////////////////////////////////////

void Saffron_Material_Destructor(Shared<Material>* material);
void Saffron_Material_SetFloat(Shared<Material>* material, MonoString* uniform, float value);
void Saffron_Material_SetTexture(Shared<Material>* material, MonoString* uniform, Shared<Texture2D>* texture);


///////////////////////////////////////////////////////////////
/// Material Instance
///////////////////////////////////////////////////////////////

void Saffron_MaterialInstance_Destructor(Shared<MaterialInstance>* instance);
void Saffron_MaterialInstance_SetFloat(Shared<MaterialInstance>* instance, MonoString* uniform, float value);
void Saffron_MaterialInstance_SetVector3(Shared<MaterialInstance>* instance, MonoString* uniform, Vector3* value);
void Saffron_MaterialInstance_SetVector4(Shared<MaterialInstance>* instance, MonoString* uniform, Vector4* value);
void Saffron_MaterialInstance_SetTexture(Shared<MaterialInstance>* instance, MonoString* uniform,
                                         Shared<Texture2D>* texture);


///////////////////////////////////////////////////////////////
/// Mesh
///////////////////////////////////////////////////////////////

Shared<Mesh>* Saffron_Mesh_Constructor(MonoString* filepath);
void Saffron_Mesh_Destructor(Shared<Mesh>* mesh);
Shared<Material>* Saffron_Mesh_GetMaterial(Shared<Mesh>* mesh);
Shared<MaterialInstance>* Saffron_Mesh_GetMaterialByIndex(Shared<Mesh>* mesh, int index);
size_t Saffron_Mesh_GetMaterialCount(Shared<Mesh>* mesh);


///////////////////////////////////////////////////////////////
/// Mesh Factory
///////////////////////////////////////////////////////////////

Shared<Mesh>* Saffron_MeshFactory_CreatePlane(float width, float height);


///////////////////////////////////////////////////////////////
/// Scene Camera
///////////////////////////////////////////////////////////////

Shared<SceneCamera>* Saffron_Camera_Constructor(uint width, uint height);
void Saffron_Camera_Destructor(Shared<SceneCamera>* camera);

uint Saffron_Camera_GetProjectionMode(Shared<SceneCamera>* camera);
void Saffron_Camera_SetProjectionMode(Shared<SceneCamera>* camera, uint mode);

//void Saffron_SceneCamera_SetPerspectiveVerticalFOV(Shared<SceneCamera> *_this, float verticalFov);
//float Saffron_SceneCamera_GetPerspectiveVerticalFOV(Shared<SceneCamera> *_this);
//void Saffron_SceneCamera_SetPerspectiveNearClip(Shared<SceneCamera> *_this, float nearClip);
//float Saffron_SceneCamera_GetPerspectiveNearClip(Shared<SceneCamera> *_this);
//void Saffron_SceneCamera_SetPerspectiveFarClip(Shared<SceneCamera> *_this, float farClip);
//float Saffron_SceneCamera_GetPerspectiveFarClip(Shared<SceneCamera> *_this);
//
//void Saffron_SceneCamera_SetOrthographicSize(Shared<SceneCamera> *_this, float size);
//float Saffron_SceneCamera_GetOrthographicSize(Shared<SceneCamera> *_this);
//void Saffron_SceneCamera_SetOrthographicNearClip(Shared<SceneCamera> *_this, float nearClip);
//float Saffron_SceneCamera_GetOrthographicNearClip(Shared<SceneCamera> *_this);
//void Saffron_SceneCamera_SetOrthographicFarClip(Shared<SceneCamera> *_this, float farClip);
//float Saffron_SceneCamera_GetOrthographicFarClip(Shared<SceneCamera> *_this);


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
/// Components
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
/// Camera Component
///////////////////////////////////////////////////////////////

void* Saffron_CameraComponent_GetCamera(ulong entityID);
void Saffron_CameraComponent_SetCamera(ulong entityID, Shared<SceneCamera>* inCamera);

///////////////////////////////////////////////////////////////
/// Script Component
///////////////////////////////////////////////////////////////

MonoString* Saffron_ScriptComponent_GetModuleName(ulong entityID);
void Saffron_ScriptComponent_SetModuleName(ulong entityID, MonoString* moduleName);


///////////////////////////////////////////////////////////////
/// Mesh Component
///////////////////////////////////////////////////////////////

void* Saffron_MeshComponent_GetMesh(ulong entityID);
void Saffron_MeshComponent_SetMesh(ulong entityID, Shared<Mesh>* inMesh);


///////////////////////////////////////////////////////////////
/// RigidBody2D Component
///////////////////////////////////////////////////////////////

void Saffron_RigidBody2DComponent_ApplyLinearImpulse(ulong entityID, Vector2* impulse, Vector2* offset, bool wake);
void Saffron_RigidBody2DComponent_GetLinearVelocity(ulong entityID, Vector2* outVelocity);
void Saffron_RigidBody2DComponent_SetLinearVelocity(ulong entityID, Vector2* velocity);


///////////////////////////////////////////////////////////////
/// Collider2D Component
///////////////////////////////////////////////////////////////

void Saffron_Collider2DComponent_GetOffset(ulong entityID, Vector2* offset);
void Saffron_Collider2DComponent_SetOffset(ulong entityID, Vector2* offset);
float Saffron_Collider2DComponent_GetDensity(ulong entityID);
void Saffron_Collider2DComponent_SetDensity(ulong entityID, float density);
float Saffron_Collider2DComponent_GetFriction(ulong entityID);
void Saffron_Collider2DComponent_SetFriction(ulong entityID, float friction);


///////////////////////////////////////////////////////////////
/// BoxCollider2D Component
///////////////////////////////////////////////////////////////

void Saffron_BoxCollider2DComponent_GetSize(ulong entityID, Vector2* size);
void Saffron_BoxCollider2DComponent_SetSize(ulong entityID, Vector2* size);


///////////////////////////////////////////////////////////////
/// CircleCollider2D Component
///////////////////////////////////////////////////////////////

float Saffron_CircleCollider2DComponent_GetRadius(ulong entityID);
void Saffron_CircleCollider2DComponent_SetRadius(ulong entityID, float radius);


///////////////////////////////////////////////////////////////
/// RigidBody3D Component
///////////////////////////////////////////////////////////////

void Saffron_RigidBody3DComponent_ApplyLinearImpulse(ulong entityID, Vector3* impulse, Vector3* offset, bool wake);
void Saffron_RigidBody3DComponent_GetLinearVelocity(ulong entityID, Vector3* outVelocity);
void Saffron_RigidBody3DComponent_SetLinearVelocity(ulong entityID, Vector3* velocity);


///////////////////////////////////////////////////////////////
/// Collider3D Component
///////////////////////////////////////////////////////////////

void Saffron_Collider3DComponent_GetOffset(ulong entityID, Vector3* offset);
void Saffron_Collider3DComponent_SetOffset(ulong entityID, Vector3* offset);
float Saffron_Collider3DComponent_GetDensity(ulong entityID);
void Saffron_Collider3DComponent_SetDensity(ulong entityID, float density);
float Saffron_Collider3DComponent_GetFriction(ulong entityID);
void Saffron_Collider3DComponent_SetFriction(ulong entityID, float friction);


///////////////////////////////////////////////////////////////
/// BoxCollider3D Component
///////////////////////////////////////////////////////////////

void Saffron_BoxCollider3DComponent_GetSize(ulong entityID, Vector3* size);
void Saffron_BoxCollider3DComponent_SetSize(ulong entityID, Vector3* size);


///////////////////////////////////////////////////////////////
/// CircleCollider3D Component
///////////////////////////////////////////////////////////////

float Saffron_SphereCollider3DComponent_GetRadius(ulong entityID);
void Saffron_SphereCollider3DComponent_SetRadius(ulong entityID, float radius);
}
}
