#pragma once

#include "Saffron/Base.h"
#include "Saffron/Editor/EditorCamera.h"
#include "Saffron/Physics/PhysicsWorld2D.h"
#include "Saffron/Physics/PhysicsWorld3D.h"

namespace Se
{

////////////////////////////////////////////////////////
/// Components for Scenes
////////////////////////////////////////////////////////

struct SceneIDComponent
{
	UUID SceneID;
};

struct PhysicsWorld2DComponent
{
	PhysicsWorld2D World;

	PhysicsWorld2DComponent(Scene &scene)
		: World(scene)
	{
	}
};

struct PhysicsWorld3DComponent
{
	PhysicsWorld3D World;

	PhysicsWorld3DComponent(Scene &scene)
		: World(scene)
	{
	}
};

struct EditorCameraComponent
{
	std::shared_ptr<EditorCamera> Camera;

	EditorCameraComponent()
		: Camera(CreateShared<EditorCamera>())
	{
	}
	EditorCameraComponent(Matrix4f projectionMatrix)
		: Camera(CreateShared<EditorCamera>(projectionMatrix))
	{
	}
	EditorCameraComponent(const EditorCameraComponent &other) = default;

	operator std::shared_ptr<EditorCamera>() const { return Camera; }
};
}
