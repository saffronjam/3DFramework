using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Se;

namespace Script
{
    public class BasicController : Entity
    {
        public float Speed;

        private Entity m_PlayerEntity;
        private CameraComponent m_CameraComponent;

        public void OnCreate()
        {
            m_PlayerEntity = FindEntityByTag("Player");
            m_CameraComponent = GetComponent<CameraComponent>();
        }

        public void OnUpdate(float ts)
        {
            Matrix4 transform = GetTransform();

            Vector3 translation = transform.Translation;
            translation.XY = m_PlayerEntity.GetTransform().Translation.XY;
            translation.Y = Math.Max(translation.Y, 4.5f);
            transform.Translation = translation;
            SetTransform(transform);


            if (Input.IsKeyPressed(KeyCode.O))
            {
                m_CameraComponent.Camera.Projection = Camera.ProjectionMode.Orthographic;
            }
            if (Input.IsKeyPressed(KeyCode.P))
            {
                m_CameraComponent.Camera.Projection = Camera.ProjectionMode.Perspective;
            }

        }


    }
}
