#include "ShaderProgram.h"

#include <glm/gtc/type_ptr.hpp>

struct ShaderProgram::UniformBinder
{
    UniformBinder(ShaderProgram &program, const std::string &name)
            : savedProgram(0),
              currentProgram(program.m_programID),
              location(-1)
    {
        if (currentProgram)
        {
            // Enable program object
            glCheck(glGetIntegerv(GL_CURRENT_PROGRAM, &savedProgram));
            if (currentProgram != savedProgram)
                glCheck(glUseProgram(currentProgram));

            // Store uniform location for further use outside constructor
            location = program.GetUniformLocation(name);
        }
    }

    ~UniformBinder()
    {
        // Disable program object
        if (currentProgram && (currentProgram != savedProgram))
            glCheck(glUseProgram(savedProgram));
    }

    GLint savedProgram{};
    GLuint currentProgram{};
    GLint location;
};

ShaderProgram::ShaderProgram()
        : m_programID(0)
{
    glCheck(m_programID = glCreateProgram());
}

ShaderProgram::ShaderProgram(const VertexShader &vert, const FragmentShader &frag)
        : m_programID(0)
{
    glCheck(m_programID = glCreateProgram());
    Attach(vert);
    Attach(frag);
    Link();
}

ShaderProgram::~ShaderProgram()
{
    if (m_programID)
        glCheck(glDeleteProgram(m_programID));
}

ShaderProgram::ShaderProgram(ShaderProgram &&other) noexcept
        :
        m_programID(other.m_programID),
        m_uniforms(other.m_uniforms)
{
    other.m_programID = 0u;
}

void ShaderProgram::BindTo(Graphics &gfx)
{
    glUseProgram(m_programID);
}

void ShaderProgram::Attach(const Shader &shader) const
{
    glCheck(glAttachShader(m_programID, shader.GetShaderID()));
}

void ShaderProgram::SetUniform(const std::string &name, float x)
{
    UniformBinder binder(*this, name);
    if (binder.location != -1)
        glCheck(glUniform1f(binder.location, x));
}

void ShaderProgram::SetUniform(const std::string &name, const glm::vec2 &v)
{
    UniformBinder binder(*this, name);
    if (binder.location != -1)
        glCheck(glUniform2f(binder.location, v.x, v.y));
}

void ShaderProgram::SetUniform(const std::string &name, const glm::vec3 &v)
{
    UniformBinder binder(*this, name);
    if (binder.location != -1)
        glCheck(glUniform3f(binder.location, v.x, v.y, v.z));
}

void ShaderProgram::SetUniform(const std::string &name, const glm::vec4 &v)
{
    UniformBinder binder(*this, name);
    if (binder.location != -1)
        glCheck(glUniform4f(binder.location, v.x, v.y, v.z, v.w));
}

void ShaderProgram::SetUniform(const std::string &name, int x)
{
    UniformBinder binder(*this, name);
    if (binder.location != -1)
        glCheck(glUniform1i(binder.location, x));
}

void ShaderProgram::SetUniform(const std::string &name, const glm::ivec2 &v)
{
    UniformBinder binder(*this, name);
    if (binder.location != -1)
        glCheck(glUniform2i(binder.location, v.x, v.y));
}

void ShaderProgram::SetUniform(const std::string &name, const glm::ivec3 &v)
{
    UniformBinder binder(*this, name);
    if (binder.location != -1)
        glCheck(glUniform3i(binder.location, v.x, v.y, v.z));
}

void ShaderProgram::SetUniform(const std::string &name, const glm::ivec4 &v)
{
    UniformBinder binder(*this, name);
    if (binder.location != -1)
        glCheck(glUniform4i(binder.location, v.x, v.y, v.z, v.w));
}

void ShaderProgram::SetUniform(const std::string &name, bool x)
{
    SetUniform(name, static_cast<int>(x));
}

void ShaderProgram::SetUniform(const std::string &name, const glm::bvec2 &v)
{
    SetUniform(name, glm::ivec2(v));
}

void ShaderProgram::SetUniform(const std::string &name, const glm::bvec3 &v)
{
    SetUniform(name, glm::ivec3(v));
}

void ShaderProgram::SetUniform(const std::string &name, const glm::bvec4 &v)
{
    SetUniform(name, glm::ivec4(v));
}

void ShaderProgram::SetUniform(const std::string &name, const glm::mat3 &matrix)
{
    UniformBinder binder(*this, name);
    if (binder.location != -1)
        glCheck(glUniformMatrix3fv(binder.location, 1, GL_FALSE, glm::value_ptr(matrix)));
}

void ShaderProgram::SetUniform(const std::string &name, const glm::mat4 &matrix)
{
    UniformBinder binder(*this, name);
    if (binder.location != -1)
        glCheck(glUniformMatrix4fv(binder.location, 1, GL_FALSE, glm::value_ptr(matrix)));
}

void ShaderProgram::Link() const
{
    // Link the program
    glCheck(glLinkProgram(m_programID));
    // Check the link log
    GLint success;
    glCheck(glGetProgramiv(m_programID, GL_LINK_STATUS, &success));
    if (success == GL_FALSE)
    {
        GLint logLength;
        glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &logLength);
        char log[logLength];
        std::memset(log, 0, logLength * sizeof(char));
        glCheck(glGetProgramInfoLog(m_programID, logLength, nullptr, log));
        log_error("Failed to link shader program (ID:%u): %s", m_programID, log);
    }
}

int ShaderProgram::GetUniformLocation(const std::string &name)
{
    // Check the cache
    auto it = m_uniforms.find(name);
    if (it != m_uniforms.end())
    {
        // Already in cache, return it
        return it->second;
    }
    else
    {
        // Not in cache, request the location from OpenGL
        int location = glGetUniformLocation(m_programID, name.c_str());
        m_uniforms.insert(std::make_pair(name, location));

        if (location == -1)
        THROW(Bindable::Exception, "Uniform \"%s\" not found in shader program (ID: %u)", name.c_str(), m_programID);

        return location;
    }
}
