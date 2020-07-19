#include "Shader.h"

#include <cstring>

Shader::Shader(Type shaderType, const char *shaderRaw)
        : m_type(shaderType),
          m_shaderID(0u)
{
    switch (m_type)
    {
    case Type::Vertex:
        m_name = "Vertex shader";
    case Type::Geometry:
        m_name = "Geometry shader";
    case Type::Fragment:
        m_name = "Fragment shader";
    default:
        m_name = "Invalid shader";
    }
    m_shaderID = glCreateShader(MapShaderType(m_type));
    glCheck(glShaderSource(m_shaderID, 1, &shaderRaw, nullptr));
    glCheck(glCompileShader(m_shaderID));

    GLint compiledOK;
    glCheck(glGetShaderiv(m_shaderID, GL_COMPILE_STATUS, &compiledOK));
    if (compiledOK == GL_FALSE)
    {
        GLint logLength;
        glCheck(glGetShaderiv(m_shaderID, GL_INFO_LOG_LENGTH, &logLength));
        char infoLog[logLength];
        std::memset(infoLog, 0, logLength * sizeof(char));
        glCheck(glGetShaderInfoLog(m_shaderID, logLength, nullptr, infoLog));
        glCheck(glDeleteShader(m_shaderID));
        THROW(Bindable::Exception, "Failed to compile %s (ID: %u): %s", GetName().c_str(), m_shaderID, infoLog);
    }
}

Shader::~Shader()
{
    glCheck(glDeleteShader(m_shaderID));
}

GLenum Shader::MapShaderType(Shader::Type type) noexcept
{
    switch (type)
    {
    case Type::Vertex:
        return GL_VERTEX_SHADER;
    case Type::Geometry:
        return GL_GEOMETRY_SHADER;
    case Type::Fragment:
        return GL_FRAGMENT_SHADER;
    default:
        return GL_INVALID_VALUE;
    }
}
const std::string &Shader::GetName() const noexcept
{
    return m_name;
}

unsigned int Shader::GetShaderID() const noexcept
{
    return m_shaderID;
}
