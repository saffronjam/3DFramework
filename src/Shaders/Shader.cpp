#include "Shader.h"

#include <GL/glew.h>

static GLenum MapShaderType(Shader::Type type) noexcept
{
    switch (type)
    {
    case Shader::Type::Vertex:
        return GL_VERTEX_SHADER;
    case Shader::Type::Geometry:
        return GL_GEOMETRY_SHADER;
    case Shader::Type::Fragment:
        return GL_FRAGMENT_SHADER;
    default:
        return GL_INVALID_VALUE;
    }
}

Shader::Shader(Shader::Type shaderType)
        : m_type(shaderType),
          m_shaderID(0u)
{
    ConfigureName();
}

Shader::Shader(Type shaderType, const std::string &filepath)
        : m_type(shaderType),
          m_shaderID(0u)
{
    ConfigureName();
    Shader::LoadFromFile(filepath);
}

Shader::~Shader()
{
    CleanUp();
}

Shader::Shader(Shader &&other) noexcept
        : FileIO(std::move(other)),
          m_type(other.m_type),
          m_shaderID(other.m_shaderID),
          m_name(std::move(other.m_name))
{
    other.m_shaderID = 0;
}

bool Shader::LoadFromFile(const std::string &path)
{
    CleanUp();

    m_filepath = path;
    FileIO::LoadFromFile(path);
    glCheck(m_shaderID = glCreateShader(MapShaderType(m_type)));
    auto pathRawString = m_fileContents.c_str();
    glCheck(glShaderSource(m_shaderID, 1, &pathRawString, nullptr));
    glCheck(glCompileShader(m_shaderID));

    GLint compiledOK;
    glCheck(glGetShaderiv(m_shaderID, GL_COMPILE_STATUS, &compiledOK));
    if (compiledOK == GL_FALSE)
    {
        GLint logLength;
        glCheck(glGetShaderiv(m_shaderID, GL_INFO_LOG_LENGTH, &logLength));
        char infoLog[logLength];
        glCheck(glGetShaderInfoLog(m_shaderID, logLength, nullptr, infoLog));

        LogErrorUser("Failed to compile %s Path: %s (ID: %u): %s", GetName().c_str(), m_filepath.c_str(), m_shaderID, infoLog);
        CleanUp();
    }
    return static_cast<bool>(m_shaderID);
}

unsigned int Shader::GetShaderID() const noexcept
{
    return m_shaderID;
}

const std::string &Shader::GetName() const noexcept
{
    return m_name;
}

const std::string &Shader::GetFilepath() const noexcept
{
    return m_filepath;
}

void Shader::ConfigureName() noexcept
{
    switch (m_type)
    {
    case Type::Vertex:
        m_name = "Vertex shader";
        break;
    case Type::Geometry:
        m_name = "Geometry shader";
        break;
    case Type::Fragment:
        m_name = "Fragment shader";
        break;
    default:
        m_name = "Invalid shader";
        break;
    }
}

void Shader::CleanUp()
{
    if (m_shaderID != 0u)
    {
        glCheck(glDeleteShader(m_shaderID));
        m_shaderID = 0u;
    }
}