#pragma once

#include "Drawable.h"

class TestTriangle : public Drawable
{
public:
    TestTriangle();

    void Update(const Mouse &mouse) override;

private:
    ShaderProgram* m_shaderProgram;
};


