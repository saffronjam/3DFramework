#include "VertexConfig.h"

#include <cassert>

// VertexElementLayout
VertexElementLayout &VertexElementLayout::Append(ElementType type) noxnd
{
    elements.emplace_back(type, Size());
    return *this;
}

const VertexElementLayout::Element &VertexElementLayout::Resolve(ElementType type) const noxnd
{
    {
        for (auto &e : elements)
        {
            if (e.GetType() == type)
            {
                return e;
            }
        }
        assert("Could not resolve element type" && false);
        return elements.front();
    }
}

const VertexElementLayout::Element &VertexElementLayout::ResolveByIndex(size_t i) const noxnd
{
    return elements[i];
}

size_t VertexElementLayout::Size() const noxnd
{
    return elements.empty() ? 0u : elements.back().GetOffsetAfter();
}

const std::vector<VertexElementLayout::Element> &VertexElementLayout::GetElements() const noexcept
{
    return elements;
}

std::string VertexElementLayout::GetCode() const noxnd
{
    std::string code;
    for (const auto &e : elements)
    {
        code += e.GetCode();
    }
    return code;
}

// VertexElementLayout::Element
VertexElementLayout::Element::Element(ElementType type, size_t offset)
        :
        type(type),
        offset(offset)
{
}

size_t VertexElementLayout::Element::GetOffsetAfter() const noxnd
{
    return offset + Size();
}

size_t VertexElementLayout::Element::GetOffset() const
{
    return offset;
}

size_t VertexElementLayout::Element::Size() const noxnd
{
    return SizeOf(type);
}

constexpr size_t VertexElementLayout::Element::SizeOf(ElementType type) noxnd
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

ElementType VertexElementLayout::Element::GetType() const noexcept
{
    return type;
}

const char *VertexElementLayout::Element::GetCode() const noexcept
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

VertexProperties VertexElementLayout::Element::GetVertexProperties() const noxnd
{
    switch (type)
    {
    case ElementType::Position2D:
        return VertexMap<ElementType::Position2D>::vertexProperties;
    case ElementType::Position3D:
        return VertexMap<ElementType::Position3D>::vertexProperties;
    case ElementType::Texture2D:
        return VertexMap<ElementType::Texture2D>::vertexProperties;
    case ElementType::Normal:
        return VertexMap<ElementType::Normal>::vertexProperties;
    case ElementType::Float3Color:
        return VertexMap<ElementType::Float3Color>::vertexProperties;
    case ElementType::Float4Color:
        return VertexMap<ElementType::Float4Color>::vertexProperties;
    case ElementType::BGRAColor:
        return VertexMap<ElementType::BGRAColor>::vertexProperties;
    default:
        assert("Invalid element type" && false);
        return VertexProperties{};
    }
}

// Vertex
Vertex::Vertex(char *pData, const VertexElementLayout &layout) noxnd
        :
        pData(pData),
        vertexElementLayout(layout)
{
    assert(pData != nullptr);
}

ConstVertex::ConstVertex(const Vertex &v) noxnd
        :
        vertex(v)
{
}

// VertexBuffer
RawVertexBuffer::RawVertexBuffer(VertexElementLayout layout, size_t size) noxnd
        :
        vertexElementLayout(std::move(layout))
{
    Resize(size);
}

const char *RawVertexBuffer::GetData() const noxnd
{
    return buffer.data();
}

const VertexElementLayout &RawVertexBuffer::GetLayout() const noexcept
{
    return vertexElementLayout;
}

size_t RawVertexBuffer::Size() const noxnd
{
    return buffer.size() / vertexElementLayout.Size();
}

size_t RawVertexBuffer::SizeBytes() const noxnd
{
    return buffer.size();
}

void RawVertexBuffer::Resize(size_t newSize) noxnd
{
    const auto size = Size();
    if (size < newSize)
    {
        buffer.resize(buffer.size() + vertexElementLayout.Size() * (newSize - size));
    }
}

Vertex RawVertexBuffer::Back() noxnd
{
    assert(!buffer.empty());
    return Vertex{buffer.data() + buffer.size() - vertexElementLayout.Size(), vertexElementLayout};
}

Vertex RawVertexBuffer::Front() noxnd
{
    assert(!buffer.empty());
    return Vertex{buffer.data(), vertexElementLayout};
}

Vertex RawVertexBuffer::operator[](size_t i) noxnd
{
    assert(i < Size());
    return Vertex{buffer.data() + vertexElementLayout.Size() * i, vertexElementLayout};
}

ConstVertex RawVertexBuffer::Back() const noxnd
{
    return ConstVertex(const_cast<RawVertexBuffer *>(this)->Back());
}

ConstVertex RawVertexBuffer::Front() const noxnd
{
    return ConstVertex(const_cast<RawVertexBuffer *>(this)->Front());
}

ConstVertex RawVertexBuffer::operator[](size_t i) const noxnd
{
    return ConstVertex(const_cast<RawVertexBuffer &>(*this)[i]);
}
