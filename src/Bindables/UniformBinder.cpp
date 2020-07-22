#include "UniformBinder.h"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

namespace Bind
{
    struct UniformBinder::UniformBindHelper
    {
        UniformBindHelper(const UniformBinder &uniformBinder, const ShaderProgram &program, const std::string &name)
                : savedProgram(0),
                  currentProgram(program.GetProgramID()),
                  location(-1)
        {
            if (currentProgram)
            {
                // Enable program object
                glCheck(glGetIntegerv(GL_CURRENT_PROGRAM, &savedProgram));
                if (currentProgram != savedProgram)
                    glCheck(glUseProgram(currentProgram));

                // Store uniform location for further use outside constructor
                location = uniformBinder.GetUniformLocation(name);
            }
        }

        ~UniformBindHelper()
        {
            // Disable program object
            if (currentProgram && (currentProgram != savedProgram))
                glCheck(glUseProgram(savedProgram));
        }

        GLint savedProgram{};
        GLuint currentProgram{};
        GLint location;
    };

    UniformBinder::UniformBinder(ShaderProgram::Ptr shaderProgram)
            : m_shaderProgram(std::move(shaderProgram))
    {
    }

    void UniformBinder::BindTo(Graphics &gfx)
    {
        for (auto&[key, uniform] : m_uniforms)
        {
            switch (uniform.dataType)
            {
            case 0:
                switch (uniform.vecType)
                {
                case 0:
                    Set_(key, uniform.f.x);
                    break;
                case 1:
                    Set_(key, uniform.f.x, uniform.f.y);
                    break;
                case 2:
                    Set_(key, uniform.f.x, uniform.f.y, uniform.f.z);
                    break;
                case 3:
                    Set_(key, uniform.f.x, uniform.f.y, uniform.f.z, uniform.f.w);
                    break;
                }
                break;
            case 1:
                switch (uniform.vecType)
                {
                case 0:
                    Set_(key, uniform.i.x);
                    break;
                case 1:
                    Set_(key, uniform.i.x, uniform.i.y);
                    break;
                case 2:
                    Set_(key, uniform.i.x, uniform.i.y, uniform.i.z);
                    break;
                case 3:
                    Set_(key, uniform.i.x, uniform.i.y, uniform.i.z, uniform.i.w);
                    break;
                }
                break;
            case 2:
                switch (uniform.vecType)
                {
                case 0:
                    Set_(key, uniform.b.x);
                    break;
                case 1:
                    Set_(key, uniform.b.x, uniform.b.y);
                    break;
                case 2:
                    Set_(key, uniform.b.x, uniform.b.y, uniform.b.z);
                    break;
                case 3:
                    Set_(key, uniform.b.x, uniform.b.y, uniform.b.z, uniform.b.w);
                    break;
                }
            case 3:
            {
                switch (uniform.vecType)
                {
                case 0:
                    Set_(key, uniform.m.m3);
                    break;
                case 1:
                    Set_(key, uniform.m.m4);
                    break;
                }
                break;
            }
            }
        }
    }

    void UniformBinder::Set(const std::string &name, const glm::mat3 &matrix)
    {
        UniformBindValue val;
        val.dataType = 3;
        val.vecType = 0;
        val.m.m3 = matrix;
        m_uniforms[name] = val;
    }

    void UniformBinder::Set(const std::string &name, const glm::mat4 &matrix)
    {
        UniformBindValue val;
        val.dataType = 3;
        val.vecType = 1;
        val.m.m4 = matrix;
        m_uniforms[name] = val;
    }

    void UniformBinder::Set_(const std::string &name, float x)
    {
        UniformBindHelper bindHelper(*this, *m_shaderProgram, name);
        if (bindHelper.location != -1)
            glCheck(glUniform1f(bindHelper.location, x));
    }

    void UniformBinder::Set_(const std::string &name, float x, float y)
    {
        UniformBindHelper bindHelper(*this, *m_shaderProgram, name);
        if (bindHelper.location != -1)
            glCheck(glUniform2f(bindHelper.location, x, y));
    }

    void UniformBinder::Set_(const std::string &name, float x, float y, float z)
    {
        UniformBindHelper bindHelper(*this, *m_shaderProgram, name);
        if (bindHelper.location != -1)
            glCheck(glUniform3f(bindHelper.location, x, y, z));
    }

    void UniformBinder::Set_(const std::string &name, float x, float y, float z, float w)
    {
        UniformBindHelper bindHelper(*this, *m_shaderProgram, name);
        if (bindHelper.location != -1)
            glCheck(glUniform4f(bindHelper.location, x, y, z, w));
    }

    void UniformBinder::Set_(const std::string &name, int x)
    {
        UniformBindHelper bindHelper(*this, *m_shaderProgram, name);
        if (bindHelper.location != -1)
            glCheck(glUniform1i(bindHelper.location, x));
    }

    void UniformBinder::Set_(const std::string &name, int x, int y)
    {
        UniformBindHelper bindHelper(*this, *m_shaderProgram, name);
        if (bindHelper.location != -1)
            glCheck(glUniform2i(bindHelper.location, x, y));
    }

    void UniformBinder::Set_(const std::string &name, int x, int y, int z)
    {
        UniformBindHelper bindHelper(*this, *m_shaderProgram, name);
        if (bindHelper.location != -1)
            glCheck(glUniform3i(bindHelper.location, x, y, z));
    }

    void UniformBinder::Set_(const std::string &name, int x, int y, int z, int w)
    {
        UniformBindHelper bindHelper(*this, *m_shaderProgram, name);
        if (bindHelper.location != -1)
            glCheck(glUniform4i(bindHelper.location, x, y, z, w));
    }

    void UniformBinder::Set_(const std::string &name, bool x)
    {
        Set_(name, static_cast<int>(x));
    }

    void UniformBinder::Set_(const std::string &name, bool x, bool y)
    {
        Set_(name, static_cast<int>(x), static_cast<int>(y));
    }

    void UniformBinder::Set_(const std::string &name, bool x, bool y, bool z)
    {
        Set_(name, static_cast<int>(x), static_cast<int>(y), static_cast<int>(z));
    }

    void UniformBinder::Set_(const std::string &name, bool x, bool y, bool z, bool w)
    {
        Set_(name, static_cast<int>(x), static_cast<int>(y), static_cast<int>(z), static_cast<int>(w));
    }

    void UniformBinder::Set_(const std::string &name, const glm::mat3 &matrix)
    {
        UniformBindHelper bindHelper(*this, *m_shaderProgram, name);
        if (bindHelper.location != -1)
            glCheck(glUniformMatrix3fv(bindHelper.location, 1, GL_FALSE, glm::value_ptr(matrix)));
    }

    void UniformBinder::Set_(const std::string &name, const glm::mat4 &matrix)
    {
        UniformBindHelper bindHelper(*this, *m_shaderProgram, name);
        if (bindHelper.location != -1)
            glCheck(glUniformMatrix4fv(bindHelper.location, 1, GL_FALSE, glm::value_ptr(matrix)));
    }

    UniformBinder::Ptr UniformBinder::Resolve(const ShaderProgram::Ptr &shaderProgram)
    {
        return Codex::Resolve<UniformBinder>(shaderProgram);
    }

    std::string UniformBinder::GetUID() const noexcept
    {
        return GenerateUID(m_shaderProgram);
    }

    int UniformBinder::GetUniformLocation(const std::string &name) const
    {
        // Check the cache
        auto it = m_uniformLocationCache.find(name);
        if (it != m_uniformLocationCache.end())
        {
            // Already in cache, return it
            return it->second;
        }
        else
        {
            // Not in cache, request the location from OpenGL
            int location = glGetUniformLocation(m_shaderProgram->GetProgramID(), name.c_str());
            m_uniformLocationCache.insert(std::make_pair(name, location));

            if (location == -1)
            THROW(Bindable::Exception, "Uniform \"%s\" not found in shader program (ID: %u)", name.c_str(), m_shaderProgram->GetProgramID());

            return location;
        }
    }

}