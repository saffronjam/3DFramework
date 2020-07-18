#include "Shader.h"

#include <cstring>

Shader::Shader()
        : m_shaderID(0u)
{
}

Shader::~Shader()
{
    if (m_shaderID != 0u)
        glDeleteShader(m_shaderID);
}

void Shader::Compile(const char *shaderRaw, GLenum shaderType)
{
    m_shaderID = glCreateShader(shaderType);
    glShaderSource(m_shaderID, 1, &shaderRaw, nullptr);
    glCompileShader(m_shaderID);

    GLint compiledOK;
    glGetShaderiv(m_shaderID, GL_COMPILE_STATUS, &compiledOK);
    if (compiledOK == GL_FALSE)
    {
        GLint logLength;
        glGetShaderiv(m_shaderID, GL_INFO_LOG_LENGTH, &logLength);
        char infoLog[logLength];
        std::memset(infoLog, 0, logLength * sizeof(char));
        glGetShaderInfoLog(m_shaderID, logLength, nullptr, infoLog);
        THROW(Bindable::Exception, "Failed to compile %s: %s", GetName().c_str(), infoLog);
    }
}

void Shader::BindTo(Graphics &gfx)
{
    glAttachShader(m_shaderProgram.value(), m_shaderID);
    glLinkProgram(m_shaderProgram.value());
    GLint linkedOK;
    glGetProgramiv(m_shaderProgram.value(), GL_LINK_STATUS, &linkedOK);
    if (linkedOK == GL_FALSE)
    {
        GLint logLength;
        glGetProgramiv(m_shaderProgram.value(), GL_INFO_LOG_LENGTH, &logLength);
        char infoLog[logLength];
        std::memset(infoLog, 0, logLength * sizeof(char));
        glGetShaderInfoLog(m_shaderID, logLength, nullptr, infoLog);
        THROW(Bindable::Exception, "Failed to link %s: %s", GetName().c_str(), infoLog);
    }
    glUseProgram(m_shaderProgram.value());
}