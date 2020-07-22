#pragma once

#include <cstdint>

#include "Bindable.h"
#include "BindableCodex.h"
#include "ShaderProgram.h"

namespace Bind
{
    class UniformBinder : public Bindable
    {
        struct UniformBindValue
        {
            int location = 0;
            int dataType = 0;
            int dataSpec = 0;
            union
            {
                union
                {
                    float v1;
                    glm::vec2 v2;
                    glm::vec3 v3;
                    glm::vec4 v4;
                    glm::mat3 m3;
                    glm::mat4 m4;
                } f;
                union
                {
                    int v1;
                    glm::ivec2 v2;
                    glm::ivec3 v3;
                    glm::ivec4 v4;
                } i;
                union
                {
                    bool v1;
                    glm::bvec2 v2;
                    glm::bvec3 v3;
                    glm::bvec4 v4;
                } b;
            };
        };

    private:
        struct UniformBindHelper;
    public:
        using Ptr = std::shared_ptr<UniformBinder>;

    public:
        explicit UniformBinder(ShaderProgram::Ptr shaderProgram);

        void BindTo(Graphics &gfx) override;
        static UniformBinder::Ptr Resolve(const ShaderProgram::Ptr &shaderProgram);

        template<typename... Ignore>
        [[nodiscard]] static std::string GenerateUID(const ShaderProgram::Ptr &shaderProgram, Ignore &&...ignore);
        [[nodiscard]] std::string GetUID() const noexcept override;

        void Set(const std::string &name, float x);
        void Set(const std::string &name, const glm::vec2 &vector);
        void Set(const std::string &name, const glm::vec3 &vector);
        void Set(const std::string &name, const glm::vec4 &vector);

        void Set(const std::string &name, int x);
        void Set(const std::string &name, const glm::ivec2 &vector);
        void Set(const std::string &name, const glm::ivec3 &vector);
        void Set(const std::string &name, const glm::ivec4 &vector);

        void Set(const std::string &name, bool x);
        void Set(const std::string &name, const glm::bvec2 &vector);
        void Set(const std::string &name, const glm::bvec3 &vector);
        void Set(const std::string &name, const glm::bvec4 &vector);

        void Set(const std::string &name, const glm::mat3 &matrix);
        void Set(const std::string &name, const glm::mat4 &matrix);

    private:
        void Set_(const std::string &name, float x);
        void Set_(const std::string &name, const glm::vec2 &vector);
        void Set_(const std::string &name, const glm::vec3 &vector);
        void Set_(const std::string &name, const glm::vec4 &vector);

        void Set_(const std::string &name, int x);
        void Set_(const std::string &name, const glm::ivec2 &vector);
        void Set_(const std::string &name, const glm::ivec3 &vector);
        void Set_(const std::string &name, const glm::ivec4 &vector);

        void Set_(const std::string &name, bool x);
        void Set_(const std::string &name, const glm::bvec2 &vector);
        void Set_(const std::string &name, const glm::bvec3 &vector);
        void Set_(const std::string &name, const glm::bvec4 &vector);

        void Set_(const std::string &name, const glm::mat3 &matrix);
        void Set_(const std::string &name, const glm::mat4 &matrix);

        int GetUniformLocation(const std::string &name) const;

    private:
        ShaderProgram::Ptr m_shaderProgram;
        std::map<std::string, UniformBindValue> m_uniforms;
        mutable std::map<std::string, int> m_uniformLocationCache;
    };

    template<typename... Ignore>
    std::string UniformBinder::GenerateUID(const ShaderProgram::Ptr &shaderProgram, Ignore &&...ignore)
    {
        static long long uid = 0;
        using namespace std::string_literals;
        return typeid(UniformBinder).name() + "#"s + std::to_string(uid++);
    }
}


