#include "Camera.h"

#include "imgui/imgui.h"

Camera::Camera(glm::vec3 position)
        : m_matrix(1),
          m_position(position),
          m_forward(0.0f, 0.0f, 0.0f),
          m_up(0.0f, 1.0f, 0.0f),
          m_right(1.0f, 0.0f, 0.0f),
          m_movementSpeed(2.5f),
          m_mouseSensitivity(0.13f),
          m_zoom(45.0f),
          m_worldUp(0.0f, 1.0f, 0.0f),
          m_yaw(-90.0f),
          m_pitch(0.0f)
{
    UpdateCameraVectors();
}

void Camera::Update(float dt, Mouse &mouse, Keyboard &kbd)
{
    // Handle position
    m_movementSpeed = kbd.IsDown(Keyboard::LeftShift) ? 8.0f : 2.5f;

    float velocity = m_movementSpeed * dt;
    if (kbd.IsDown(Keyboard::W))
        m_position += m_forward * velocity;
    if (kbd.IsDown(Keyboard::S))
        m_position -= m_forward * velocity;
    if (kbd.IsDown(Keyboard::A))
        m_position -= m_right * velocity;
    if (kbd.IsDown(Keyboard::D))
        m_position += m_right * velocity;

    // Handle yaw and pitch
    if (mouse.IsAnyDown() && !ImGui::GetIO().WantCaptureMouse)
    {
        glm::vec2 mouseDelta = mouse.GetDelta() * m_mouseSensitivity;
        m_yaw -= mouseDelta.x;
        m_pitch += mouseDelta.y;
        m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);
    }

    // Handle zoom
//    m_zoom -= 0.0f;
//    m_zoom = std::clamp(m_zoom, 1.0f, 45.0f);

    // Final updates
    UpdateCameraVectors();
    m_matrix = glm::lookAt(m_position, m_position + m_forward, m_up);
}

const glm::vec3 &Camera::GetPosition() const noexcept
{
    return m_position;
}

const glm::vec3 &Camera::GetForward() const noexcept
{
    return m_forward;
}


const glm::mat4 &Camera::GetMatrix() const noexcept
{
    return m_matrix;
}

void Camera::UpdateCameraVectors()
{
    // Calculate the new Front vector
    glm::vec4 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_forward = glm::normalize(front);
    // also re-calculate the Right and Up vector
    m_right = glm::normalize(glm::cross(m_forward, m_worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    m_up = glm::normalize(glm::cross(m_right, m_forward));
}
