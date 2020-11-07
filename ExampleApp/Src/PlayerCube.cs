using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Se;

namespace Script
{
    class PlayerCube : Entity
    {
        public float HorizontalForce = 50.0f;
        public float JumpForce = 2000.0f;

        private RigidBody3DComponent m_PhysicsBody;
        private MaterialInstance m_MeshMaterial;

        private int m_CollisionCounter = 0;

        public Vector3 MaxSpeed = new Vector3();

        private bool Colliding => m_CollisionCounter > 0;

        void OnCreate()
        {
            m_PhysicsBody = GetComponent<RigidBody3DComponent>();

            var meshComponent = GetComponent<MeshComponent>();
            m_MeshMaterial = meshComponent.Mesh.GetMaterial(0); ;

            AddCollision3DBeginCallback(OnPlayerCollisionBegin);
            AddCollision3DEndCallback(OnPlayerCollisionEnd);
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
            float movementForce = HorizontalForce;

            //if (!Colliding)
            //{
            //    movementForce *= 0.4f;
            //}

            if (Input.IsKeyPressed(KeyCode.D))
            {
                m_PhysicsBody.ApplyLinearImpulse(new Vector3(movementForce, 0, 0), new Vector3());
            }
            else if (Input.IsKeyPressed(KeyCode.A))
            {
                m_PhysicsBody.ApplyLinearImpulse(new Vector3(-movementForce, 0, 0), new Vector3());
            }


            if (Colliding && Input.IsMouseButtonPressed(MouseButtonCode.Right))
            {
                m_PhysicsBody.ApplyLinearImpulse(new Vector3(0, JumpForce, 0), new Vector3());
            }

            if (m_CollisionCounter > 0)
            {
                m_MeshMaterial.Set("u_AlbedoColor", new Vector3(1.0f, 0.0f, 0.0f));
            }
            else
            {
                m_MeshMaterial.Set("u_AlbedoColor", new Vector3(0.8f, 0.8f, 0.8f));
            }

            Vector3 linearVelocity = m_PhysicsBody.LinearVelocity;
            linearVelocity.Clamp(-MaxSpeed, MaxSpeed);
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
