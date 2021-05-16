using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Saffron;

namespace Script
{
    public class PlayerCube : Entity
    {
        public float HorizontalForce = 10.0f;
        public float JumpForce = 10.0f;
        public Vector2 TestVec = new Vector2(1);

        private RigidBody2DComponent m_PhysicsBody;
        private MaterialInstance m_MeshMaterial;

        private int m_CollisionCounter = 0;

        public Vector2 MaxSpeed = new Vector2();

        private bool Colliding => m_CollisionCounter > 0;

        public override void OnCreate()
        {
            m_PhysicsBody = GetComponent<RigidBody2DComponent>();

            var meshComponent = GetComponent<MeshComponent>();
            m_MeshMaterial = meshComponent.Mesh.GetMaterial(0);
            m_MeshMaterial.Set("u_Metalness", 0.0f);

            m_MeshMaterial.Set("u_AlbedoColor", new Vector3(1.0f, 0.0f, 0.0f));

            AddCollision2DBeginCallback(OnPlayerCollisionBegin);
            AddCollision2DEndCallback(OnPlayerCollisionEnd);
        }

        public void OnPlayerCollisionBegin(float value)
        {
            m_CollisionCounter++;
        }

        public void OnPlayerCollisionEnd(float value)
        {
            m_CollisionCounter--;
        }

        public override void OnUpdate(float dt)
        {
            var movementForce = HorizontalForce;

            if (!Colliding)
            {
                movementForce *= 0.4f;
            }

            if (Input.IsKeyDown(KeyCode.D))
                m_PhysicsBody.ApplyLinearImpulse(new Vector2(movementForce, 0), new Vector2(), true);
            else if (Input.IsKeyDown(KeyCode.A))
                m_PhysicsBody.ApplyLinearImpulse(new Vector2(-movementForce, 0), new Vector2(), true);

            if (Colliding && Input.IsKeyDown(KeyCode.Space))
                m_PhysicsBody.ApplyLinearImpulse(new Vector2(0, JumpForce), new Vector2(0, 0), true);

            if (m_CollisionCounter > 0)
                m_MeshMaterial.Set("u_AlbedoColor", new Vector3(1.0f, 0.0f, 0.0f));
            else
                m_MeshMaterial.Set("u_AlbedoColor", new Vector3(0.8f, 0.8f, 0.8f));

            var linearVelocity = m_PhysicsBody.LinearVelocity;
            linearVelocity.Clamp(new Vector2(-MaxSpeed.X, -1000), MaxSpeed);
            m_PhysicsBody.LinearVelocity = linearVelocity;

            if (Input.IsKeyPressed(KeyCode.R))
            {
                var transform = GetTransform();
                transform.Translation = new Vector3(0.0f);
                SetTransform(transform);
            }
        }
    }
}