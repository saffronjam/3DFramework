using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Se;

namespace Example
{
    class PlayerCube : Entity
    {
        public float HorizontalForce = 10.0f;
        public float JumpForce = 10.0f;

        private RigidBody2DComponent m_PhysicsBody;
        private MaterialInstance m_MeshMaterial;
        private ScriptComponent m_ScriptComponent;
        private CircleCollider2DComponent m_CircleCollider2DComponent;

        int m_CollisionCounter = 0;

        public Vector2 MaxSpeed = new Vector2();

        private bool Colliding => m_CollisionCounter > 0;

        void OnCreate()
        {
            m_ScriptComponent = GetComponent<ScriptComponent>();
            m_CircleCollider2DComponent = GetComponent<CircleCollider2DComponent>();

            m_PhysicsBody = GetComponent<RigidBody2DComponent>();

            MeshComponent meshComponent = GetComponent<MeshComponent>();
            m_MeshMaterial = meshComponent.Mesh.GetMaterial(0); ;

            AddCollision2DBeginCallback(OnPlayerCollisionBegin);
            AddCollision2DEndCallback(OnPlayerCollisionEnd);
        }

        void OnPlayerCollisionBegin(float value)
        {
            m_CollisionCounter++;
        }

        void OnPlayerCollisionEnd(float value)
        {
            m_CollisionCounter--;
        }
        void OnUpdate(float ts)
        {
            if (Input.IsKeyPressed(KeyCode.B))
                m_CircleCollider2DComponent.Radius = 0.0f;

            float movementForce = HorizontalForce;

            if (!Colliding)
            {
                movementForce *= 0.4f;
            }
            if (Input.IsKeyPressed(KeyCode.D))
                m_PhysicsBody.ApplyLinearImpulse(new Vector2(movementForce, 0), new Vector2(), true);
            else if (Input.IsKeyPressed(KeyCode.A))
                m_PhysicsBody.ApplyLinearImpulse(new Vector2(-movementForce, 0), new Vector2(), true);


            if (Colliding && Input.IsMouseButtonPressed(MouseButtonCode.Right))
                m_PhysicsBody.ApplyLinearImpulse(new Vector2(0, JumpForce), new Vector2(0, 0), true);

            if (m_CollisionCounter > 0)
                m_MeshMaterial.Set("u_AlbedoColor", new Vector3(1.0f, 0.0f, 0.0f));
            else
                m_MeshMaterial.Set("u_AlbedoColor", new Vector3(0.8f, 0.8f, 0.8f));

            Vector2 linearVelocity = m_PhysicsBody.LinearVelocity;
            linearVelocity.Clamp(new Vector2(-MaxSpeed.X, -1000), MaxSpeed);
            m_PhysicsBody.LinearVelocity = linearVelocity;

            if (Input.IsKeyPressed(KeyCode.R))
            {
                Matrix4 transform = GetTransform();
                transform.Translation = new Vector3(0.0f);
                SetTransform(transform);
            }

        }

    }
}
