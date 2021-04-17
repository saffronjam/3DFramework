#pragma once

#include "Saffron/Base.h"

namespace reactphysics3d
{
class PhysicsCommon;
class PhysicsWorld;
}

namespace Se
{
class Scene;

class PhysicsWorld3D
{
public:
	explicit PhysicsWorld3D(Scene& scene);
	~PhysicsWorld3D();
	PhysicsWorld3D(const PhysicsWorld3D& world);
	PhysicsWorld3D(PhysicsWorld3D&& world) noexcept;
	PhysicsWorld3D& operator=(const PhysicsWorld3D& world);

	void OnUpdate();
	void OnGuiRender();
	void OnStart();
	void OnStop();

	Vector3f GetGravity() const;
	void SetGravity(const Vector3f& gravity);

private:
	reactphysics3d::PhysicsWorld* _nativeWorld;
	Scene* _scene;
	class Entity* _physicsBodyEntityBuffer;
	bool _filledWorld = false;
	float _deltaTimeAccumulator = 0.0f;
};
}
