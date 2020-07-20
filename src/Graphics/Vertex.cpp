#include "Vertex.h"

#include <cassert>

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
        case ElementType::Position2D:
            return sizeof(VertexMap<ElementType::Position2D>::SysType);
        case ElementType::Position3D:
            return sizeof(VertexMap<ElementType::Position3D>::SysType);
        case ElementType::Texture2D:
            return sizeof(VertexMap<ElementType::Texture2D>::SysType);
        case ElementType::Normal:
            return sizeof(VertexMap<ElementType::Normal>::SysType);
        case ElementType::Float3Color:
            return sizeof(VertexMap<ElementType::Float3Color>::SysType);
        case ElementType::Float4Color:
            return sizeof(VertexMap<ElementType::Float4Color>::SysType);
        case ElementType::BGRAColor:
            return sizeof(VertexMap<ElementType::BGRAColor>::SysType);
        }
        return 0u;
    }

    ElementType VertexLayout::Element::GetType() const noexcept
    {
        return type;
    }

    const char *Dvtx::VertexLayout::Element::GetCode() const noexcept
    {
        switch (type)
        {
        case ElementType::Position2D:
            return VertexMap<ElementType::Position2D>::code;
        case ElementType::Position3D:
            return VertexMap<ElementType::Position3D>::code;
        case ElementType::Texture2D:
            return VertexMap<ElementType::Texture2D>::code;
        case ElementType::Normal:
            return VertexMap<ElementType::Normal>::code;
        case ElementType::Float3Color:
            return VertexMap<ElementType::Float3Color>::code;
        case ElementType::Float4Color:
            return VertexMap<ElementType::Float4Color>::code;
        case ElementType::BGRAColor:
            return VertexMap<ElementType::BGRAColor>::code;
        }
        assert("Invalid element type" && false);
        return "Invalid";
    }

    VertexProperties VertexLayout::Element::GetVertexProperties() const noxnd
    {
        switch (type)
        {
        case ElementType::Position2D:
            return VertexMap<ElementType::Position2D>::vertexProperties;
        case ElementType::Position3D:
            return VertexMap<ElementType::Position2D>::vertexProperties;
        case ElementType::Texture2D:
            return VertexMap<ElementType::Position2D>::vertexProperties;
        case ElementType::Normal:
            return VertexMap<ElementType::Position2D>::vertexProperties;
        case ElementType::Float3Color:
            return VertexMap<ElementType::Position2D>::vertexProperties;
        case ElementType::Float4Color:
            return VertexMap<ElementType::Position2D>::vertexProperties;
        case ElementType::BGRAColor:
            return VertexMap<ElementType::Position2D>::vertexProperties;
        default:
            assert("Invalid element type" && false);
            return VertexProperties{};
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
        return ConstVertex(const_cast<VertexBuffer *>(this)->Back());
    }

    ConstVertex VertexBuffer::Front() const noxnd
    {
        return ConstVertex(const_cast<VertexBuffer *>(this)->Front());
    }

    ConstVertex VertexBuffer::operator[](size_t i) const noxnd
    {
        return ConstVertex(const_cast<VertexBuffer &>(*this)[i]);
    }
}