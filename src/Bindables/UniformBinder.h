#pragma once

#include <variant>

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
            int vecType = 0;
            union
            {
                struct
                {
                    float x,y,z,w;
                } f;
                struct
                {
                    int x,y,z,w;
                } i;
                struct
                {
                    bool x,y,z,w;
                } b;
                struct
                {
                    union{
                        glm::mat3 m3;
                        glm::mat4 m4;
                    };
                } m;
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

        template<typename T>
        void Set(const std::string &name, const T &val);
        template<size_t size, typename T>
        void Set(const std::string &name, const glm::vec<size, T> &vector);

        void Set(const std::string &name, const glm::mat3 &matrix);
        void Set(const std::string &name, const glm::mat4 &matrix);

    private:
        template<size_t N, typename... Args>
        auto ParamsIndex(Args &&... as) noexcept
        {
            return std::get<N>(std::forward_as_tuple(std::forward<Args>(as)...));
        }

        template<typename T, typename... Params>
        void CopyData(const std::string &name, T data, Params &&...params)
        {
            constexpr size_t nParams = sizeof...(Params);
            constexpr size_t dataSize = sizeof(T);
            auto &uniformBindValue = m_uniforms[name];
            uniformBindValue.vecType = nParams + 1;
            char *dataPointer = nullptr;
            if constexpr (std::is_same<T, float>::value)
            {
                uniformBindValue.dataType = 0;
                dataPointer = reinterpret_cast<char *>(&uniformBindValue.f);
            }
            else if constexpr (std::is_same<T, int>::value)
            {
                uniformBindValue.dataType = 1;
                dataPointer = reinterpret_cast<char *>(&uniformBindValue.i);
            }
            else
            {
                uniformBindValue.dataType = 2;
                dataPointer = reinterpret_cast<char *>(&uniformBindValue.b);
            }

            memcpy(dataPointer, &data, dataSize);
            if constexpr (nParams > 0)
            {
                float lVal = ParamsIndex<0>(std::forward<Params>(params)...);
                memcpy(dataPointer + dataSize, &lVal, dataSize);
            }
            if constexpr (nParams > 1)
            {
                float lVal = ParamsIndex<1>(std::forward<Params>(params)...);
                memcpy(dataPointer + dataSize * 2, &lVal, dataSize);
            }
            if constexpr (nParams > 2)
            {
                float lVal = ParamsIndex<2>(std::forward<Params>(params)...);
                memcpy(dataPointer + dataSize * 3, &lVal, dataSize);
            }
        }

        void Set_(const std::string &name, float x);
        void Set_(const std::string &name, float x, float y);
        void Set_(const std::string &name, float x, float y, float z);
        void Set_(const std::string &name, float x, float y, float z, float w);

        void Set_(const std::string &name, int x);
        void Set_(const std::string &name, int x, int y);
        void Set_(const std::string &name, int x, int y, int z);
        void Set_(const std::string &name, int x, int y, int z, int w);

        void Set_(const std::string &name, bool x);
        void Set_(const std::string &name, bool x, bool y);
        void Set_(const std::string &name, bool x, bool y, bool z);
        void Set_(const std::string &name, bool x, bool y, bool z, bool w);

        void Set_(const std::string &name, const glm::mat3 &matrix);
        void Set_(const std::string &name, const glm::mat4 &matrix);

        int GetUniformLocation(const std::string &name) const;

    private:
        ShaderProgram::Ptr m_shaderProgram;
        std::map<std::string, UniformBindValue> m_uniforms;
        mutable std::map<std::string, int> m_uniformLocationCache;
    };

    template<typename T>
    void UniformBinder::Set(const std::string &name, const T &val)
    {
        CopyData(name, val);
    }

    template<size_t size, typename T>
    void UniformBinder::Set(const std::string &name, const glm::vec<size, T> &vector)
    {
        static_assert(size == 2 || size == 3 || size == 4);
        if constexpr(size == 2)
        {
            CopyData(name, vector.x, vector.y);
        }
        else if constexpr(size == 3)
        {
            CopyData(name, vector.x, vector.y, vector.z);
        }
        else if constexpr(size == 4)
        {
            CopyData(name, vector.x, vector.y, vector.z, vector.w);
        }

    }

    template<typename... Ignore>
    std::string UniformBinder::GenerateUID(const ShaderProgram::Ptr &shaderProgram, Ignore &&...ignore)
    {
        static long long uid = 0;
        using namespace std::string_literals;
        return typeid(UniformBinder).name() + "#"s + std::to_string(uid++);
    }
}


