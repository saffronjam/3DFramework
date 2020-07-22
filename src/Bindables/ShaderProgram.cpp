#include "ShaderProgram.h"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

namespace Bind
{

    ShaderProgram::ShaderProgram(VertexShader &vert, FragmentShader &frag)
            : m_vertexShader(std::move(vert)),
              m_fragmentShader(std::move(frag)),
              m_shaderProgramID(0u)
    {
        glCheck(m_shaderProgramID = glCreateProgram());
        Attach(m_vertexShader);
        Attach(m_fragmentShader);
        Link();
    }

    ShaderProgram::~ShaderProgram()
    {
        if (m_shaderProgramID != 0)
            glCheck(glDeleteProgram(m_shaderProgramID));
    }

    unsigned int ShaderProgram::GetProgramID() const noexcept
    {
        return m_shaderProgramID;
    }

    void ShaderProgram::BindTo(Graphics &gfx)
    {
        glCheck(glUseProgram(m_shaderProgramID));
    }

    ShaderProgram::Ptr ShaderProgram::Resolve(VertexShader &vert, FragmentShader &frag)
    {
        return Codex::Resolve<ShaderProgram>(vert, frag);
    }

    std::string ShaderProgram::GetUID() const noexcept
    {
        return ShaderProgram::GenerateUID(m_vertexShader, m_fragmentShader);
    }

    void ShaderProgram::Attach(const Shader &shader) const
    {
        glCheck(glAttachShader(m_shaderProgramID, shader.GetShaderID()));
    }

    void ShaderProgram::Link() const
    {
        // Link the program
        glCheck(glLinkProgram(m_shaderProgramID));
        // Check the link log
        GLint success;
        glCheck(glGetProgramiv(m_shaderProgramID, GL_LINK_STATUS, &success));
        if (success == GL_FALSE)
        {
            GLint logLength;
            glGetProgramiv(m_shaderProgramID, GL_INFO_LOG_LENGTH, &logLength);
            char log[logLength];
            std::memset(log, 0, logLength * sizeof(char));
            glCheck(glGetProgramInfoLog(m_shaderProgramID, logLength, nullptr, log));
            log_error("Failed to link shader program (ID:%u): %s", m_shaderProgramID, log);
        }
    }
}