#pragma once

#include <map>
#include <string>

#include "SeMath.h"
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

    public:
        using Ptr = std::shared_ptr<ShaderProgram>;

    public:
        ShaderProgram(VertexShader &vert, FragmentShader &frag);
        ~ShaderProgram();

        [[nodiscard]] unsigned int GetProgramID() const noexcept;

        void BindTo(Graphics &gfx) override;
        static ShaderProgram::Ptr Resolve(VertexShader &vert, FragmentShader &frag);

        template<typename... Ignore>
        [[nodiscard]] static std::string GenerateUID(const VertexShader &vert, const FragmentShader &frag, Ignore &&...ignore);
        [[nodiscard]] std::string GetUID() const noexcept override;

    private:
        void Attach(const Shader &shader) const;
        bool Link();
        void CleanUp();


    private:
        VertexShader m_vertexShader;
        FragmentShader m_fragmentShader;

        unsigned int m_shaderProgramID;
    };

    template<typename... Ignore>
    std::string ShaderProgram::GenerateUID(const VertexShader &vert, const FragmentShader &frag, Ignore &&...ignore)
    {
        using namespace std::string_literals;
        return typeid(ShaderProgram).name() + "#"s + vert.GetFilepath() + frag.GetFilepath();
    }
}