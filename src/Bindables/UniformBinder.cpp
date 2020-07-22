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
                switch (uniform.dataSpec)
                {
                case 0:
                    Set_(key, uniform.f.v1);
                    break;
                case 1:
                    Set_(key, uniform.f.v2);
                    break;
                case 2:
                    Set_(key, uniform.f.v3);
                    break;
                case 3:
                    Set_(key, uniform.f.v4);
                    break;
                case 4:
                    Set_(key, uniform.f.m3);
                    break;
                case 5:
                    Set_(key, uniform.f.m4);
                    break;
                }
                break;
            case 1:
                switch (uniform.dataSpec)
                {
                case 0:
                    Set_(key, uniform.i.v1);
                    break;
                case 1:
                    Set_(key, uniform.i.v2);
                    break;
                case 2:
                    Set_(key, uniform.i.v3);
                    break;
                case 3:
                    Set_(key, uniform.i.v4);
                    break;
                }
                break;
            case 2:
                switch (uniform.dataSpec)
                {
                case 0:
                    Set_(key, uniform.b.v1);
                    break;
                case 1:
                    Set_(key, uniform.b.v2);
                    break;
                case 2:
                    Set_(key, uniform.b.v3);
                    break;
                case 3:
                    Set_(key, uniform.b.v4);
                    break;
                }
                break;
            }
        }
    }

    void UniformBinder::Set(const std::string &name, float x)
    {
        UniformBindValue val;
        val.dataType = 0;
        val.dataSpec = 0;
        val.f.v1 = x;
        m_uniforms[name] = val;
    }

    void UniformBinder::Set(const std::string &name, const glm::vec2 &vector)
    {
        UniformBindValue val;
        val.dataType = 0;
        val.dataSpec = 1;
        val.f.v2 = vector;
        m_uniforms[name] = val;
    }

    void UniformBinder::Set(const std::string &name, const glm::vec3 &vector)
    {
        UniformBindValue val;
        val.dataType = 0;
        val.dataSpec = 2;
        val.f.v3 = vector;
        m_uniforms[name] = val;
    }

    void UniformBinder::Set(const std::string &name, const glm::vec4 &vector)
    {
        UniformBindValue val;
        val.dataType = 0;
        val.dataSpec = 3;
        val.f.v4 = vector;
        m_uniforms[name] = val;
    }

    void UniformBinder::Set(const std::string &name, int x)
    {
        UniformBindValue val;
        val.dataType = 1;
        val.dataSpec = 0;
        val.i.v1 = x;
        m_uniforms[name] = val;
    }

    void UniformBinder::Set(const std::string &name, const glm::ivec2 &vector)
    {
        UniformBindValue val;
        val.dataType = 1;
        val.dataSpec = 1;
        val.i.v2 = vector;
        m_uniforms[name] = val;
    }

    void UniformBinder::Set(const std::string &name, const glm::ivec3 &vector)
    {
        UniformBindValue val;
        val.dataType = 1;
        val.dataSpec = 2;
        val.i.v3 = vector;
        m_uniforms[name] = val;
    }

    void UniformBinder::Set(const std::string &name, const glm::ivec4 &vector)
    {
        UniformBindValue val;
        val.dataType = 1;
        val.dataSpec = 3;
        val.i.v4 = vector;
        m_uniforms[name] = val;
    }

    void UniformBinder::Set(const std::string &name, bool x)
    {
        UniformBindValue val;
        val.dataType = 2;
        val.dataSpec = 0;
        val.b.v1 = x;
        m_uniforms[name] = val;
    }

    void UniformBinder::Set(const std::string &name, const glm::bvec2 &vector)
    {
        UniformBindValue val;
        val.dataType = 2;
        val.dataSpec = 1;
        val.b.v2 = vector;
        m_uniforms[name] = val;
    }

    void UniformBinder::Set(const std::string &name, const glm::bvec3 &vector)
    {
        UniformBindValue val;
        val.dataType = 2;
        val.dataSpec = 2;
        val.b.v3 = vector;
        m_uniforms[name] = val;
    }

    void UniformBinder::Set(const std::string &name, const glm::bvec4 &vector)
    {
        UniformBindValue val;
        val.dataType = 2;
        val.dataSpec = 3;
        val.b.v4 = vector;
        m_uniforms[name] = val;
    }

    void UniformBinder::Set(const std::string &name, const glm::mat3 &matrix)
    {
        UniformBindValue val;
        val.dataType = 0;
        val.dataSpec = 4;
        val.f.m3 = matrix;
        m_uniforms[name] = val;
    }

    void UniformBinder::Set(const std::string &name, const glm::mat4 &matrix)
    {
        UniformBindValue val;
        val.dataType = 0;
        val.dataSpec = 5;
        val.f.m4 = matrix;
        m_uniforms[name] = val;
    }

    void UniformBinder::Set_(const std::string &name, float x)
    {
        UniformBindHelper bindHelper(*this, *m_shaderProgram, name);
        if (bindHelper.location != -1)
            glCheck(glUniform1f(bindHelper.location, x));
    }

    void UniformBinder::Set_(const std::string &name, const glm::vec2 &v)
    {
        UniformBindHelper bindHelper(*this, *m_shaderProgram, name);
        if (bindHelper.location != -1)
            glCheck(glUniform2f(bindHelper.location, v.x, v.y));
    }

    void UniformBinder::Set_(const std::string &name, const glm::vec3 &v)
    {
        UniformBindHelper bindHelper(*this, *m_shaderProgram, name);
        if (bindHelper.location != -1)
            glCheck(glUniform3f(bindHelper.location, v.x, v.y, v.z));
    }

    void UniformBinder::Set_(const std::string &name, const glm::vec4 &v)
    {
        UniformBindHelper bindHelper(*this, *m_shaderProgram, name);
        if (bindHelper.location != -1)
            glCheck(glUniform4f(bindHelper.location, v.x, v.y, v.z, v.w));
    }

    void UniformBinder::Set_(const std::string &name, int x)
    {
        UniformBindHelper bindHelper(*this, *m_shaderProgram, name);
        if (bindHelper.location != -1)
            glCheck(glUniform1i(bindHelper.location, x));
    }

    void UniformBinder::Set_(const std::string &name, const glm::ivec2 &v)
    {
        UniformBindHelper bindHelper(*this, *m_shaderProgram, name);
        if (bindHelper.location != -1)
            glCheck(glUniform2i(bindHelper.location, v.x, v.y));
    }

    void UniformBinder::Set_(const std::string &name, const glm::ivec3 &v)
    {
        UniformBindHelper bindHelper(*this, *m_shaderProgram, name);
        if (bindHelper.location != -1)
            glCheck(glUniform3i(bindHelper.location, v.x, v.y, v.z));
    }

    void UniformBinder::Set_(const std::string &name, const glm::ivec4 &v)
    {
        UniformBindHelper bindHelper(*this, *m_shaderProgram, name);
        if (bindHelper.location != -1)
            glCheck(glUniform4i(bindHelper.location, v.x, v.y, v.z, v.w));
    }

    void UniformBinder::Set_(const std::string &name, bool x)
    {
        Set(name, static_cast<int>(x));
    }

    void UniformBinder::Set_(const std::string &name, const glm::bvec2 &v)
    {
        Set(name, glm::ivec2(v));
    }

    void UniformBinder::Set_(const std::string &name, const glm::bvec3 &v)
    {
        Set(name, glm::ivec3(v));
    }

    void UniformBinder::Set_(const std::string &name, const glm::bvec4 &v)
    {
        Set(name, glm::ivec4(v));
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