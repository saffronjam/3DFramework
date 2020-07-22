#pragma once

#include <vector>

#include "Bindable.h"
#include "BindableCodex.h"
#include "VertexConfig.h"

namespace Bind
{
    class VertexBuffer : public Bindable
    {
    public:
        using Ptr = std::shared_ptr<VertexBuffer>;

    public:
        VertexBuffer(std::string tag, RawVertexBuffer &rawVertexBuffer);
        ~VertexBuffer();

        void BindTo(Graphics &gfx) override;
        static VertexBuffer::Ptr Resolve(const std::string &tag, RawVertexBuffer &rawVertexBuffer);

        template<typename... Ignore>
        [[nodiscard]] static std::string GenerateUID(const std::string &tag, Ignore &&...ignore);
        [[nodiscard]] std::string GetUID() const noexcept override;

    private:
        std::string m_tag;
        unsigned int m_vboID;
        unsigned int m_vaoID;
    };

    template<typename... Ignore>
    std::string VertexBuffer::GenerateUID(const std::string &tag, Ignore &&...ignore)
    {
        using namespace std::string_literals;
        return typeid(VertexBuffer).name() + "#"s + tag;
    }
}


