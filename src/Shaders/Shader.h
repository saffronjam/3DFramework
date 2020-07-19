#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/matrix.hpp>

#include "Bindable.h"

class Shader
{
protected:
    enum Type
    {
        Vertex,
        Geometry,
        Fragment
    };

public:
    explicit Shader(Type shaderType, const char *shaderRaw);
    ~Shader();

public:
    [[nodiscard]] const std::string &GetName() const noexcept;
    [[nodiscard]] unsigned int GetShaderID() const noexcept;

private:
    static GLenum MapShaderType(Type type) noexcept;

private:
    Type m_type;
    unsigned int m_shaderID;
    std::string m_name;
};