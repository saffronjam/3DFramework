#pragma once

#include <optional>
#include <GL/glew.h>

class ShaderProgram
{
public:
    ShaderProgram();
protected:
    static std::optional<unsigned int> m_shaderProgram;
};


