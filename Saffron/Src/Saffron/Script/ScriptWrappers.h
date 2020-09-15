#pragma once

#include "Saffron/Input/KeyCodes.h"
#include "Saffron/Renderer/Mesh.h"
#include "Saffron/Renderer/Material.h"
#include "Saffron/Script/ScriptEngine.h"
#include "Saffron/System/SaffronMath.h"

extern "C" {
	typedef struct _MonoString MonoString;
	typedef struct _MonoArray MonoArray;
}


namespace Se
{
namespace Script {
// Input
bool Saffron_Input_IsKeyPressed(const Keyboard &keyboard, KeyCode key);

// Entity
void Saffron_Entity_GetTransform(uint64_t entityID, glm::mat4 *outTransform);
void Saffron_Entity_SetTransform(uint64_t entityID, glm::mat4 *inTransform);
void Saffron_Entity_CreateComponent(uint64_t entityID, void *type);
bool Saffron_Entity_HasComponent(uint64_t entityID, void *type);
uint64_t Saffron_Entity_FindEntityByTag(MonoString *tag);

void *Saffron_MeshComponent_GetMesh(uint64_t entityID);
void Saffron_MeshComponent_SetMesh(uint64_t entityID, Ref<Mesh> *inMesh);

void Saffron_RigidBody2DComponent_ApplyLinearImpulse(uint64_t entityID, glm::vec2 *impulse, glm::vec2 *offset, bool wake);
void Saffron_RigidBody2DComponent_GetLinearVelocity(uint64_t entityID, glm::vec2 *outVelocity);
void Saffron_RigidBody2DComponent_SetLinearVelocity(uint64_t entityID, glm::vec2 *velocity);

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
}
}

