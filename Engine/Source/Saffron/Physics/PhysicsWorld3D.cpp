#include "SaffronPCH.h"

#include <reactphysics3d/reactphysics3d.h>

#include "Saffron/Core/Global.h"
#include "Saffron/Core/Misc.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Physics/PhysicsWorld3D.h"
#include "Saffron/Scene/Scene.h"
#include "Saffron/Script/ScriptEngine.h"

namespace Se
{

class ContactListener3D : public reactphysics3d::EventListener
{
public:

	void onContact(const CallbackData &callbackData) override
	{
		for ( int i = 0; i < callbackData.getNbContactPairs(); i++ )
		{
			const auto pair = callbackData.getContactPair(i);
			const auto *body1 = pair.getBody1();
			const auto *body2 = pair.getBody2();

			Entity &entity1 = *static_cast<Entity *>(body1->getUserData());
			Entity &entity2 = *static_cast<Entity *>(body2->getUserData());

			switch ( pair.getEventType() )
			{
			case ContactPair::EventType::ContactStart:
				if ( entity1.HasComponent<ScriptComponent>() && ScriptEngine::ModuleExists(entity1.GetComponent<ScriptComponent>().ModuleName) )
					ScriptEngine::OnCollision3DBegin(entity1);
				if ( entity2.HasComponent<ScriptComponent>() && ScriptEngine::ModuleExists(entity2.GetComponent<ScriptComponent>().ModuleName) )
					ScriptEngine::OnCollision3DBegin(entity2);
				break;
			case ContactPair::EventType::ContactStay:
				break;
			case ContactPair::EventType::ContactExit:
				if ( entity1.HasComponent<ScriptComponent>() && ScriptEngine::ModuleExists(entity1.GetComponent<ScriptComponent>().ModuleName) )
					ScriptEngine::OnCollision3DEnd(entity1);
				if ( entity2.HasComponent<ScriptComponent>() && ScriptEngine::ModuleExists(entity2.GetComponent<ScriptComponent>().ModuleName) )
					ScriptEngine::OnCollision3DEnd(entity2);
				break;
			}
		}

	}

	[[maybe_unused]] void onTrigger(const reactphysics3d::OverlapCallback::CallbackData &callbackData) override
	{

	}
};

static ContactListener3D s_ContactListener3D;

reactphysics3d::PhysicsCommon s_PhysicsCommon;

PhysicsWorld3D::PhysicsWorld3D(Scene &scene)
	:
	m_Scene(&scene),
	m_PhysicsBodyEntityBuffer(nullptr)
{
	reactphysics3d::PhysicsWorld::WorldSettings settings;
	settings.defaultVelocitySolverNbIterations = 20;
	settings.isSleepingEnabled = false;
	settings.gravity = reactphysics3d::Vector3(0, -9.81f, 0);
	m_NativeWorld = s_PhysicsCommon.createPhysicsWorld(settings);

	SetGravity({ 0.0f, -9.82f, 0.0f });
	m_NativeWorld->setEventListener(&s_ContactListener3D);
}

PhysicsWorld3D::~PhysicsWorld3D()
{
	OnStop();
}

PhysicsWorld3D::PhysicsWorld3D(const PhysicsWorld3D &world)
	: m_NativeWorld(world.m_NativeWorld),
	m_Scene(world.m_Scene),
	m_PhysicsBodyEntityBuffer(nullptr)
{
}

PhysicsWorld3D::PhysicsWorld3D(PhysicsWorld3D &&world) noexcept
	: m_NativeWorld(world.m_NativeWorld),
	m_Scene(world.m_Scene),
	m_PhysicsBodyEntityBuffer(world.m_PhysicsBodyEntityBuffer)
{
	world.m_NativeWorld = nullptr;
	world.m_Scene = nullptr;
	world.m_PhysicsBodyEntityBuffer = nullptr;
}

PhysicsWorld3D &PhysicsWorld3D::operator=(const PhysicsWorld3D &world)
{
	if ( &world == this )
		return *this;

	m_NativeWorld = world.m_NativeWorld;
	m_Scene = world.m_Scene;
	m_PhysicsBodyEntityBuffer = nullptr;

	return *this;
}

void PhysicsWorld3D::OnUpdate()
{
	const float contantTimeStep = 1.0f / 144.0f;
	const auto ts = Global::Timer::GetStep();
	m_DeltaTimeAccumulator += ts.sec();
	while ( m_DeltaTimeAccumulator >= contantTimeStep )
	{
		m_NativeWorld->update(contantTimeStep);
		m_DeltaTimeAccumulator -= contantTimeStep;
	}

	{
		auto view = m_Scene->GetEntityRegistry().view<RigidBody3DComponent>();
		for ( auto entityHandle : view )
		{
			Entity entity = { entityHandle, m_Scene };

			auto &transform = entity.Transform();
			auto [position, rotation, scale] = Misc::GetTransformDecomposition(transform);
			auto &rigidBody = entity.GetComponent<RigidBody3DComponent>();
			auto *body = static_cast<reactphysics3d::RigidBody *>(rigidBody.RuntimeBody);

			if ( entity.HasComponent<BoxCollider3DComponent>() )
			{
				auto &component = entity.GetComponent<BoxCollider3DComponent>();
				auto *fixture = static_cast<reactphysics3d::Collider *>(component.RuntimeFixture);

				fixture->getMaterial().setMassDensity(component.Density);
				fixture->getMaterial().setFrictionCoefficient(component.Friction);
			}
			if ( entity.HasComponent<SphereCollider3DComponent>() )
			{
				auto &component = entity.GetComponent<SphereCollider3DComponent>();
				auto *fixture = static_cast<reactphysics3d::Collider *>(component.RuntimeFixture);

				fixture->getMaterial().setMassDensity(component.Density);
				fixture->getMaterial().setFrictionCoefficient(component.Friction);
			}

			const auto &bodyTransform = body->getTransform();
			const auto &bodyPosition = bodyTransform.getPosition();
			const auto &bodyQuatRotation = bodyTransform.getOrientation();

			transform = glm::translate(Vector3f{ bodyPosition.x, bodyPosition.y, bodyPosition.z }) *
				glm::toMat4(Quaternion{ bodyQuatRotation.x, bodyQuatRotation.y, bodyQuatRotation.z, bodyQuatRotation.w }) *
				glm::scale(scale);
		}
	}
}

void PhysicsWorld3D::OnGuiRender()
{
	ImGui::Begin("Physics World 3D");
	Gui::BeginPropertyGrid();
	auto gravity = GetGravity();
	if ( Gui::Property("Gravity", gravity, -10000.0f, 10000.0f, 1.0f, Gui::PropertyFlag::Drag) )
	{
		SetGravity(gravity);
	}
	Gui::EndPropertyGrid();
	ImGui::End();
}

void PhysicsWorld3D::OnStart()
{
	if ( !m_FilledWorld )
	{
		{
			auto view = m_Scene->GetEntityRegistry().view<RigidBody3DComponent>();
			m_PhysicsBodyEntityBuffer = new Entity[view.size()];
			Uint32 physicsBodyEntityBufferIndex = 0;
			for ( auto entityHandle : view )
			{
				Entity entity = { entityHandle, m_Scene };
				auto &transform = entity.Transform();
				auto [position, rotation, scale] = Misc::GetTransformDecomposition(transform);
				auto &rigidBody3DComponent = entity.GetComponent<RigidBody3DComponent>();

				auto *newRigidBody3D = m_NativeWorld->createRigidBody(reactphysics3d::Transform({ position.x, position.y, position.z }, { rotation.z, rotation.y, -rotation.x, rotation.w }));
				switch ( rigidBody3DComponent.BodyType )
				{
				case RigidBody3DComponent::Type::Dynamic:
					newRigidBody3D->setType(reactphysics3d::BodyType::DYNAMIC);
					break;
				case RigidBody3DComponent::Type::Kinematic:
					newRigidBody3D->setType(reactphysics3d::BodyType::KINEMATIC);
					break;
				case RigidBody3DComponent::Type::Static:
					newRigidBody3D->setType(reactphysics3d::BodyType::STATIC);
					break;
				}

				Entity *entityStorage = &m_PhysicsBodyEntityBuffer[physicsBodyEntityBufferIndex++];
				*entityStorage = entity;
				newRigidBody3D->setUserData(static_cast<void *>(entityStorage));
				rigidBody3DComponent.RuntimeBody = newRigidBody3D;
			}
		}

		{
			auto view = m_Scene->GetEntityRegistry().view<BoxCollider3DComponent>();
			for ( auto entityHandle : view )
			{
				Entity entity = { entityHandle, m_Scene };

				if ( entity.HasComponent<RigidBody3DComponent>() )
				{
					auto &rigidBody3DComponent = entity.GetComponent<RigidBody3DComponent>();
					auto &boxCollider3DComponent = entity.GetComponent<BoxCollider3DComponent>();
					auto &transformComponent = entity.GetComponent<TransformComponent>();

					auto rot = Misc::GetTransformDecomposition(transformComponent.Transform).Rotation;

					SE_CORE_ASSERT(rigidBody3DComponent.RuntimeBody);
					auto *body = static_cast<reactphysics3d::RigidBody *>(rigidBody3DComponent.RuntimeBody);

					const auto halfBoxSize = boxCollider3DComponent.Size * 0.5f;
					auto *boxShape = s_PhysicsCommon.createBoxShape({ halfBoxSize.x, halfBoxSize.y, halfBoxSize.z });

					const auto &boxOffset = boxCollider3DComponent.Offset;

					auto *newFixture = body->addCollider(boxShape, reactphysics3d::Transform({ boxOffset.x, boxOffset.y, boxOffset.z }, reactphysics3d::Quaternion(rot.x, rot.y, rot.z, rot.w)));
					newFixture->getMaterial().setMassDensity(boxCollider3DComponent.Density);
					newFixture->getMaterial().setFrictionCoefficient(boxCollider3DComponent.Friction);

					boxCollider3DComponent.RuntimeFixture = newFixture;
				}
			}
		}

		{
			auto view = m_Scene->GetEntityRegistry().view<SphereCollider3DComponent>();
			for ( auto entityHandle : view )
			{
				Entity entity = { entityHandle, m_Scene };

				if ( entity.HasComponent<RigidBody3DComponent>() )
				{
					auto &rigidBody3DComponent = entity.GetComponent<RigidBody3DComponent>();
					auto &circleCollider3DComponent = entity.GetComponent<SphereCollider3DComponent>();

					SE_CORE_ASSERT(rigidBody3DComponent.RuntimeBody);
					auto *body = static_cast<reactphysics3d::RigidBody *>(rigidBody3DComponent.RuntimeBody);

					auto *boxShape = s_PhysicsCommon.createSphereShape(circleCollider3DComponent.Radius);

					const auto &boxOffset = circleCollider3DComponent.Offset;

					auto *newFixture = body->addCollider(boxShape, reactphysics3d::Transform({ boxOffset.x, boxOffset.y, boxOffset.z }, reactphysics3d::Quaternion::fromEulerAngles({ 0.0f, 0.0f, 0.0f })));
					newFixture->getMaterial().setMassDensity(circleCollider3DComponent.Density);
					newFixture->getMaterial().setFrictionCoefficient(circleCollider3DComponent.Friction);

					circleCollider3DComponent.RuntimeFixture = newFixture;
				}
			}
		}
		m_FilledWorld = true;
	}
}

void PhysicsWorld3D::OnStop()
{
	if ( m_FilledWorld )
	{
		while ( m_NativeWorld->getNbRigidBodies() > 0 )
		{
			m_NativeWorld->destroyRigidBody(m_NativeWorld->getRigidBody(0));
		}
		m_FilledWorld = false;
	}
}

Vector3f PhysicsWorld3D::GetGravity() const
{
	const auto &gravity = m_NativeWorld->getGravity();
	return { gravity.x, gravity.y, gravity.z };
}

void PhysicsWorld3D::SetGravity(const Vector3f &gravity)
{
	m_NativeWorld->setGravity({ gravity.x, gravity.y, gravity.z });
}
}
