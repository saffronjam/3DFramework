#pragma once

#include "Drawable.h"
#include "Cube.h"

class TestBox : public Drawable
{
public:
    TestBox();

    void Update(const Mouse &mouse) override;

    void Rotate(float yaw, float pitch, float roll) override;

private:
    glm::mat4 m_transform;
    ShaderProgram *m_shaderProgram;
};
