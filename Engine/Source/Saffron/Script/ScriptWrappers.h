#pragma once

#include "Saffron/Input/InputCodes.h"
#include "Saffron/Rendering/Mesh.h"
#include "Saffron/Rendering/Material.h"
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
bool Saffron_Input_IsMouseButtonPressed(MouseButtonCode mouseButton);


///////////////////////////////////////////////////////////////
/// Entity
///////////////////////////////////////////////////////////////

void Saffron_Entity_GetTransform(Uint64 entityID, Matrix4f *outTransform);
void Saffron_Entity_SetTransform(Uint64 entityID, Matrix4f *inTransform);
void Saffron_Entity_CreateComponent(Uint64 entityID, void *type);
bool Saffron_Entity_HasComponent(Uint64 entityID, void *type);
Uint64 Saffron_Entity_GetEntity(MonoString *tag);


///////////////////////////////////////////////////////////////
/// Texture2D
///////////////////////////////////////////////////////////////

Shared<Texture2D> *Saffron_Texture2D_Constructor(Uint32 width, Uint32 height);
void Saffron_Texture2D_Destructor(Shared<Texture2D> *texture);
void Saffron_Texture2D_SetData(Shared<Texture2D> *texture, MonoArray *data, Int32 count);


///////////////////////////////////////////////////////////////
/// Material
///////////////////////////////////////////////////////////////

void Saffron_Material_Destructor(Shared<Material> *material);
void Saffron_Material_SetFloat(Shared<Material> *material, MonoString *uniform, float value);
void Saffron_Material_SetTexture(Shared<Material> *material, MonoString *uniform, Shared<Texture2D> *texture);


///////////////////////////////////////////////////////////////
/// Material Instance
///////////////////////////////////////////////////////////////

void Saffron_MaterialInstance_Destructor(Shared<MaterialInstance> *instance);
void Saffron_MaterialInstance_SetFloat(Shared<MaterialInstance> *instance, MonoString *uniform, float value);
void Saffron_MaterialInstance_SetVector3(Shared<MaterialInstance> *instance, MonoString *uniform, Vector3f *value);
void Saffron_MaterialInstance_SetVector4(Shared<MaterialInstance> *instance, MonoString *uniform, Vector4f *value);
void Saffron_MaterialInstance_SetTexture(Shared<MaterialInstance> *instance, MonoString *uniform, Shared<Texture2D> *texture);


///////////////////////////////////////////////////////////////
/// Mesh
///////////////////////////////////////////////////////////////

Shared<Mesh> *Saffron_Mesh_Constructor(MonoString *filepath);
void Saffron_Mesh_Destructor(Shared<Mesh> *mesh);
Shared<Material> *Saffron_Mesh_GetMaterial(Shared<Mesh> *mesh);
Shared<MaterialInstance> *Saffron_Mesh_GetMaterialByIndex(Shared<Mesh> *mesh, int index);
size_t Saffron_Mesh_GetMaterialCount(Shared<Mesh> *mesh);


///////////////////////////////////////////////////////////////
/// Mesh Factory
///////////////////////////////////////////////////////////////

Shared<Mesh> *Saffron_MeshFactory_CreatePlane(float width, float height);


///////////////////////////////////////////////////////////////
/// Scene Camera
///////////////////////////////////////////////////////////////

Shared<SceneCamera> *Saffron_Camera_Constructor(Uint32 width, Uint32 height);
void Saffron_Camera_Destructor(Shared<SceneCamera> *camera);

Uint32 Saffron_Camera_GetProjectionMode(Shared<SceneCamera> *camera);
void Saffron_Camera_SetProjectionMode(Shared<SceneCamera> *camera, Uint32 mode);

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

void *Saffron_CameraComponent_GetCamera(Uint64 entityID);
void Saffron_CameraComponent_SetCamera(Uint64 entityID, Shared<SceneCamera> *inCamera);

///////////////////////////////////////////////////////////////
/// Script Component
///////////////////////////////////////////////////////////////

MonoString *Saffron_ScriptComponent_GetModuleName(Uint64 entityID);
void Saffron_ScriptComponent_SetModuleName(Uint64 entityID, MonoString *moduleName);


///////////////////////////////////////////////////////////////
/// Mesh Component
///////////////////////////////////////////////////////////////

void *Saffron_MeshComponent_GetMesh(Uint64 entityID);
void Saffron_MeshComponent_SetMesh(Uint64 entityID, Shared<Mesh> *inMesh);


///////////////////////////////////////////////////////////////
/// RigidBody2D Component
///////////////////////////////////////////////////////////////

void Saffron_RigidBody2DComponent_ApplyLinearImpulse(Uint64 entityID, Vector2f *impulse, Vector2f *offset, bool wake);
void Saffron_RigidBody2DComponent_GetLinearVelocity(Uint64 entityID, Vector2f *outVelocity);
void Saffron_RigidBody2DComponent_SetLinearVelocity(Uint64 entityID, Vector2f *velocity);


///////////////////////////////////////////////////////////////
/// Collider2D Component
///////////////////////////////////////////////////////////////

void Saffron_Collider2DComponent_GetOffset(Uint64 entityID, Vector2f *offset);
void Saffron_Collider2DComponent_SetOffset(Uint64 entityID, Vector2f *offset);
float Saffron_Collider2DComponent_GetDensity(Uint64 entityID);
void Saffron_Collider2DComponent_SetDensity(Uint64 entityID, float density);
float Saffron_Collider2DComponent_GetFriction(Uint64 entityID);
void Saffron_Collider2DComponent_SetFriction(Uint64 entityID, float friction);


///////////////////////////////////////////////////////////////
/// BoxCollider2D Component
///////////////////////////////////////////////////////////////

void Saffron_BoxCollider2DComponent_GetSize(Uint64 entityID, Vector2f *size);
void Saffron_BoxCollider2DComponent_SetSize(Uint64 entityID, Vector2f *size);


///////////////////////////////////////////////////////////////
/// CircleCollider2D Component
///////////////////////////////////////////////////////////////

float Saffron_CircleCollider2DComponent_GetRadius(Uint64 entityID);
void Saffron_CircleCollider2DComponent_SetRadius(Uint64 entityID, float radius);


///////////////////////////////////////////////////////////////
/// RigidBody3D Component
///////////////////////////////////////////////////////////////

void Saffron_RigidBody3DComponent_ApplyLinearImpulse(Uint64 entityID, Vector3f *impulse, Vector3f *offset, bool wake);
void Saffron_RigidBody3DComponent_GetLinearVelocity(Uint64 entityID, Vector3f *outVelocity);
void Saffron_RigidBody3DComponent_SetLinearVelocity(Uint64 entityID, Vector3f *velocity);


///////////////////////////////////////////////////////////////
/// Collider3D Component
///////////////////////////////////////////////////////////////

void Saffron_Collider3DComponent_GetOffset(Uint64 entityID, Vector3f *offset);
void Saffron_Collider3DComponent_SetOffset(Uint64 entityID, Vector3f *offset);
float Saffron_Collider3DComponent_GetDensity(Uint64 entityID);
void Saffron_Collider3DComponent_SetDensity(Uint64 entityID, float density);
float Saffron_Collider3DComponent_GetFriction(Uint64 entityID);
void Saffron_Collider3DComponent_SetFriction(Uint64 entityID, float friction);


///////////////////////////////////////////////////////////////
/// BoxCollider3D Component
///////////////////////////////////////////////////////////////

void Saffron_BoxCollider3DComponent_GetSize(Uint64 entityID, Vector3f *size);
void Saffron_BoxCollider3DComponent_SetSize(Uint64 entityID, Vector3f *size);


///////////////////////////////////////////////////////////////
/// CircleCollider3D Component
///////////////////////////////////////////////////////////////

float Saffron_SphereCollider3DComponent_GetRadius(Uint64 entityID);
void Saffron_SphereCollider3DComponent_SetRadius(Uint64 entityID, float radius);

}
}

