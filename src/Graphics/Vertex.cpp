#include "Vertex.h"

namespace Dvtx
{
    // VertexLayout
    const VertexLayout::Element &VertexLayout::ResolveByIndex(size_t i) const noxnd
    {
        return elements[i];
    }

    VertexLayout &VertexLayout::Append(ElementType type) noxnd
    {
        elements.emplace_back(type, Size());
        return *this;
    }

    size_t VertexLayout::Size() const noxnd
    {
        return elements.empty() ? 0u : elements.back().GetOffsetAfter();
    }

    size_t VertexLayout::GetElementCount() const noexcept
    {
        return elements.size();
    }

    std::string VertexLayout::GetCode() const noxnd
    {
        std::string code;
        for (const auto &e : elements)
        {
            code += e.GetCode();
        }
        return code;
    }

    // VertexLayout::Element
    VertexLayout::Element::Element(ElementType type, size_t offset)
            :
            type(type),
            offset(offset) {}
    size_t VertexLayout::Element::GetOffsetAfter() const noxnd
    {
        return offset + Size();
    }
    size_t VertexLayout::Element::GetOffset() const
    {
        return offset;
    }
    size_t VertexLayout::Element::Size() const noxnd
    {
        return SizeOf(type);
    }
    constexpr size_t VertexLayout::Element::SizeOf(ElementType type) noxnd
    {
        switch (type)
        {
        case Position2D:
            return sizeof(Map<Position2D>::SysType);
        case Position3D:
            return sizeof(Map<Position3D>::SysType);
        case Texture2D:
            return sizeof(Map<Texture2D>::SysType);
        case Normal:
            return sizeof(Map<Normal>::SysType);
        case Float3Color:
            return sizeof(Map<Float3Color>::SysType);
        case Float4Color:
            return sizeof(Map<Float4Color>::SysType);
        case BGRAColor:
            return sizeof(Map<BGRAColor>::SysType);
        }
        assert("Invalid element type" && false);
        return 0u;
    }

    VertexLayout::ElementType VertexLayout::Element::GetType() const noexcept
    {
        return type;
    }

    const char *Dvtx::VertexLayout::Element::GetCode() const noexcept
    {
        switch (type)
        {
        case Position2D:
            return Map<Position2D>::code;
        case Position3D:
            return Map<Position3D>::code;
        case Texture2D:
            return Map<Texture2D>::code;
        case Normal:
            return Map<Normal>::code;
        case Float3Color:
            return Map<Float3Color>::code;
        case Float4Color:
            return Map<Float4Color>::code;
        case BGRAColor:
            return Map<BGRAColor>::code;
        }
        assert("Invalid element type" && false);
        return "Invalid";
    }

    VertexLayout::Attributes VertexLayout::Element::GetAttributes() const noxnd
    {
        switch (type)
        {
        case Position2D:
            return Map<Position2D>::attributes;
        case Position3D:
            return Map<Position2D>::attributes;
        case Texture2D:
            return Map<Position2D>::attributes;
        case Normal:
            return Map<Position2D>::attributes;
        case Float3Color:
            return Map<Position2D>::attributes;
        case Float4Color:
            return Map<Position2D>::attributes;
        case BGRAColor:
            return Map<Position2D>::attributes;
        default:
            assert("Invalid element type" && false);
            return Attributes{};
        }
    }

    // Vertex
    Vertex::Vertex(char *pData, const VertexLayout &layout) noxnd
            :
            pData(pData),
            layout(layout)
    {
        assert(pData != nullptr);
    }

    ConstVertex::ConstVertex(const Vertex &v) noxnd
            :
            vertex(v)
    {
    }

    // VertexBuffer
    VertexBuffer::VertexBuffer(VertexLayout layout) noxnd
            :
            layout(std::move(layout))
    {
    }

    const char *VertexBuffer::GetData() const noxnd
    {
        return buffer.data();
    }

    const VertexLayout &VertexBuffer::GetLayout() const noexcept
    {
        return layout;
    }

    size_t VertexBuffer::Size() const noxnd
    {
        return buffer.size() / layout.Size();
    }

    size_t VertexBuffer::SizeBytes() const noxnd
    {
        return buffer.size();
    }

    Vertex VertexBuffer::Back() noxnd
    {
        assert(!buffer.empty());
        return Vertex{buffer.data() + buffer.size() - layout.Size(), layout};
    }

    Vertex VertexBuffer::Front() noxnd
    {
        assert(!buffer.empty());
        return Vertex{buffer.data(), layout};
    }

    Vertex VertexBuffer::operator[](size_t i) noxnd
    {
        assert(i < Size());
        return Vertex{buffer.data() + layout.Size() * i, layout};
    }

    ConstVertex VertexBuffer::Back() const noxnd
    {
        return const_cast<VertexBuffer *>(this)->Back();
    }

    ConstVertex VertexBuffer::Front() const noxnd
    {
        return const_cast<VertexBuffer *>(this)->Front();
    }

    ConstVertex VertexBuffer::operator[](size_t i) const noxnd
    {
        return const_cast<VertexBuffer &>(*this)[i];
    }
}