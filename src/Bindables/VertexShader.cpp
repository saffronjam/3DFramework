#include "VertexShader.h"

VertexShader::VertexShader(const char *shaderRaw)
        : m_name("Vertex shader")
{
    Compile(shaderRaw, GL_VERTEX_SHADER);
}

const std::string &VertexShader::GetName() const noexcept
{
    return m_name;
}