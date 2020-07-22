#pragma once

#include "Bindable.h"

using IndexVector = std::vector<unsigned int>;

namespace Bind
{
    class IndexBuffer : public Bindable
    {
    public:
        using Ptr = std::shared_ptr<IndexBuffer>;
    public:
        explicit IndexBuffer(const std::string &tag = "?");
        IndexBuffer(std::string tag, IndexVector indices);
        ~IndexBuffer();

        [[nodiscard]] const IndexVector &GetIndices() const noexcept;
        void SetIndices(IndexVector indices) noexcept;

        void BindTo(Graphics &gfx) override;
        static IndexBuffer::Ptr Resolve(const std::string &tag, const IndexVector &indices);

        template<typename... Ignore>
        [[nodiscard]] static std::string GenerateUID(const std::string &tag, Ignore &&...ignore);
        [[nodiscard]] std::string GetUID() const noexcept override;

    private:
        std::string m_tag;
        unsigned int m_iboID;
        IndexVector m_indices;
    };

    template<typename... Ignore>
    std::string IndexBuffer::GenerateUID(const std::string &tag, Ignore &&...ignore)
    {
        using namespace std::string_literals;
        return typeid(IndexBuffer).name() + "#"s + tag;
    }
}