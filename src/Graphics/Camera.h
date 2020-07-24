#pragma once

#include <vector>

#include "Mouse.h"
#include "Keyboard.h"
#include "Event.h"
#include "SeMath.h"

class Camera
{
public:
    enum class Movement
    {
        Forward,
        Backward,
        Left,
        Right
    };

public:
    explicit Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));

    void Update(float dt, Mouse &mouse, Keyboard &kbd);

    [[nodiscard]] const glm::vec3 &GetPosition() const noexcept;
    [[nodiscard]] const glm::vec3 &GetForward() const noexcept;
    [[nodiscard]] const glm::mat4 &GetMatrix() const noexcept;

private:
    // Calculates the front vector from the Camera's (updated) Euler Angles
    void UpdateCameraVectors();

private:
    // Camera Attributes
    glm::mat4 m_matrix;
    glm::vec3 m_position;
    glm::vec3 m_forward;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;

    // Euler Angles
    float m_yaw;
    float m_pitch;

    // Camera options
    float m_movementSpeed;
    float m_mouseSensitivity;
    float m_zoom;

};