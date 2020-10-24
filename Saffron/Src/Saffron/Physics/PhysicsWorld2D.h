#pragma once

#include "Saffron/Core/Math/SaffronMath.h"

class b2World;

namespace Se
{
class Scene;

class PhysicsWorld2D
{
public:
	explicit PhysicsWorld2D(Scene &scene);
	~PhysicsWorld2D();
	PhysicsWorld2D(const PhysicsWorld2D &world);
	PhysicsWorld2D(PhysicsWorld2D &&world) noexcept;
	PhysicsWorld2D &operator=(const PhysicsWorld2D &world);

	void OnUpdate();
	void OnGuiRender();
	void OnStart();
	void OnStop();

	Vector2f GetGravity() const;
	void SetGravity(const Vector2f &gravity);

private:
	b2World *m_NativeWorld;
	Scene *m_Scene;
	class Entity *m_PhysicsBodyEntityBuffer;
	bool m_FilledWorld = false;
};
}


