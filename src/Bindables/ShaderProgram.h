#pragma once

#include <map>
#include <string>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/matrix.hpp>

#include "Bindable.h"
#include "VertexShader.h"
//#include "GeometryShader.h"
#include "FragmentShader.h"

class ShaderProgram : public Bindable
{
    friend class Shader;
    struct UniformBinder;
    typedef std::map<std::string, int> UniformTable;
public:
    ShaderProgram();
    ShaderProgram(const VertexShader &vert, const FragmentShader &frag);
    ~ShaderProgram();
    ShaderProgram(const ShaderProgram& other) = delete;
    ShaderProgram& operator()(const ShaderProgram& other) = delete;
    ShaderProgram(ShaderProgram&& other) noexcept;

    void BindTo(Graphics &gfx) override;

    void Attach(const Shader &shader) const;

    void SetUniform(const std::string &name, float x);
    void SetUniform(const std::string &name, const glm::vec2 &vector);
    void SetUniform(const std::string &name, const glm::vec3 &vector);
    void SetUniform(const std::string &name, const glm::vec4 &vector);

    void SetUniform(const std::string &name, int x);
    void SetUniform(const std::string &name, const glm::ivec2 &vector);
    void SetUniform(const std::string &name, const glm::ivec3 &vector);
    void SetUniform(const std::string &name, const glm::ivec4 &vector);

    void SetUniform(const std::string &name, bool x);
    void SetUniform(const std::string &name, const glm::bvec2 &vector);
    void SetUniform(const std::string &name, const glm::bvec3 &vector);
    void SetUniform(const std::string &name, const glm::bvec4 &vector);

    void SetUniform(const std::string &name, const glm::mat3 &matrix);
    void SetUniform(const std::string &name, const glm::mat4 &matrix);

private:
    void Link() const;
    int GetUniformLocation(const std::string &name);

    GLuint m_programID;
    UniformTable m_uniforms;
};