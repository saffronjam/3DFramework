#pragma once

#include "Drawable.h"
#include "Cube.h"

class TestBox : public Drawable
{
public:
    TestBox();

    void Update(const Mouse &mouse) override;

private:
    ShaderProgram *m_shaderProgram;
};
