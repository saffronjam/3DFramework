#pragma once

#include <map>
#include <string>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/matrix.hpp>

#include "Bindable.h"
#include "BindableCodex.h"
#include "VertexShader.h"
//#include "GeometryShader.h"
#include "FragmentShader.h"

namespace Bind
{
    class ShaderProgram : public Bindable
    {
        friend class Shader;

        struct UniformBinder;
        using UniformTable = std::map<std::string, int>;

    public:
        using Ptr = std::shared_ptr<ShaderProgram>;

    public:
        ShaderProgram(VertexShader &vert, FragmentShader &frag);
        ~ShaderProgram();

        void BindTo(Graphics &gfx) override;
        static ShaderProgram::Ptr Resolve(VertexShader &vert, FragmentShader &frag);

        template<typename... Ignore>
        [[nodiscard]] static std::string GenerateUID(const VertexShader &vert, const FragmentShader &frag, Ignore &&...ignore);
        [[nodiscard]] std::string GetUID() const noexcept override;

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
        void Attach(const Shader &shader) const;
        void Link() const;
        int GetUniformLocation(const std::string &name);

    private:
        VertexShader m_vertexShader;
        FragmentShader m_fragmentShader;

        unsigned int m_shaderProgramID;
        UniformTable m_uniforms;
    };

    template<typename... Ignore>
    std::string ShaderProgram::GenerateUID(const VertexShader &vert, const FragmentShader &frag, Ignore &&...ignore)
    {
        static int test = 0;
        using namespace std::string_literals;
        return typeid(ShaderProgram).name() + "#"s + vert.GetFilepath() + frag.GetFilepath() + std::to_string(test++);
    }
}