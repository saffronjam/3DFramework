#include "ShaderProgram.h"

std::optional<unsigned int> ShaderProgram::m_shaderProgram;

ShaderProgram::ShaderProgram()
{
    if (!m_shaderProgram.has_value())
    {
        m_shaderProgram = glCreateProgram();
    }
}
