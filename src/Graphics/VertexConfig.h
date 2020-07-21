#pragma once

#include <vector>
#include <type_traits>

#include <GL/glew.h>

#include "SeMath.h"
#include "Graphics.h"
#include "Color.h"
#include "Config.h"

enum class ElementType
{
    Position2D,
    Position3D,
    Texture2D,
    Normal,
    Float3Color,
    Float4Color,
    BGRAColor,
};

struct VertexProperties
{
    GLint size;
    GLenum type;
    GLboolean normalized;
    GLsizei stride;
};

template<ElementType>
struct VertexMap;

template<>
struct VertexMap<ElementType::Position2D>
{
    using SysType = glm::vec2;
    static constexpr VertexProperties vertexProperties = {2, GL_FLOAT, false, 2 * sizeof(float)};
    static constexpr const char *semantic = "Position";
    static constexpr const char *code = "P2";
};
template<>
struct VertexMap<ElementType::Position3D>
{
    using SysType = glm::vec3;
    static constexpr VertexProperties vertexProperties = {3, GL_FLOAT, false, 3 * sizeof(float)};
    static constexpr const char *semantic = "Position";
    static constexpr const char *code = "P3";
};
template<>
struct VertexMap<ElementType::Texture2D>
{
    using SysType = glm::vec2;
    static constexpr VertexProperties vertexProperties = {2, GL_FLOAT, false, 2 * sizeof(float)};
    static constexpr const char *semantic = "Texcoord";
    static constexpr const char *code = "T2";
};
template<>
struct VertexMap<ElementType::Normal>
{
    using SysType = glm::vec3;
    static constexpr VertexProperties vertexProperties = {3, GL_FLOAT, false, 3 * sizeof(float)};
    static constexpr const char *semantic = "Normal";
    static constexpr const char *code = "N";
};
template<>
struct VertexMap<ElementType::Float3Color>
{
    using SysType = glm::vec3;
    static constexpr VertexProperties vertexProperties = {3, GL_FLOAT, false, 3 * sizeof(float)};
    static constexpr const char *semantic = "Color";
    static constexpr const char *code = "C3";
};
template<>
struct VertexMap<ElementType::Float4Color>
{
    using SysType = glm::vec4;
    static constexpr VertexProperties vertexProperties = {4, GL_FLOAT, false, 4 * sizeof(float)};
    static constexpr const char *semantic = "Color";
    static constexpr const char *code = "C4";
};
template<>
struct VertexMap<ElementType::BGRAColor>
{
    using SysType = ::BGRAColor;
    static constexpr VertexProperties vertexProperties = {3, GL_BYTE, true, 3 * sizeof(char)};
    static constexpr const char *semantic = "Color";
    static constexpr const char *code = "C8";
};

class VertexElementLayout
{
public:
    class Element
    {
    public:
        Element(ElementType type, size_t offset);
        [[nodiscard]] size_t GetOffsetAfter() const noxnd;
        [[nodiscard]] size_t GetOffset() const;
        [[nodiscard]] size_t Size() const noxnd;
        static constexpr size_t SizeOf(ElementType type) noxnd;
        [[nodiscard]] ElementType GetType() const noexcept;
        [[nodiscard]] VertexProperties GetVertexProperties() const noxnd;
        [[nodiscard]] const char *GetCode() const noexcept;

    private:
        ElementType type;
        size_t offset;
    };

public:
    VertexElementLayout &Append(ElementType type) noxnd;
    [[nodiscard]] const Element &Resolve(ElementType type) const noxnd;
    [[nodiscard]] const Element &ResolveByIndex(size_t i) const noxnd;
    [[nodiscard]] size_t Size() const noxnd;
    [[nodiscard]] const std::vector<Element> &GetElements() const noexcept;
    [[nodiscard]] std::string GetCode() const noxnd;
private:
    std::vector<Element> elements;
};

class Vertex
{
    friend class RawVertexBuffer;

public:
    template<ElementType Type>
    auto &Attr() noxnd
    {
        auto pAttribute = pData + vertexElementLayout.Resolve(Type).GetOffset();
        return *reinterpret_cast<typename VertexMap<Type>::SysType *>(pAttribute);
    }
    template<typename T>
    void SetAttributeByIndex(size_t i, T &&val) noxnd
    {
        const auto &element = vertexElementLayout.ResolveByIndex(i);
        auto pAttribute = pData + element.GetOffset();
        switch (element.GetType())
        {
        case ElementType::Position2D:
            SetAttribute<ElementType::Position2D>(pAttribute, std::forward<T>(val));
            break;
        case ElementType::Position3D:
            SetAttribute<ElementType::Position3D>(pAttribute, std::forward<T>(val));
            break;
        case ElementType::Texture2D:
            SetAttribute<ElementType::Texture2D>(pAttribute, std::forward<T>(val));
            break;
        case ElementType::Normal:
            SetAttribute<ElementType::Normal>(pAttribute, std::forward<T>(val));
            break;
        case ElementType::Float3Color:
            SetAttribute<ElementType::Float3Color>(pAttribute, std::forward<T>(val));
            break;
        case ElementType::Float4Color:
            SetAttribute<ElementType::Float4Color>(pAttribute, std::forward<T>(val));
            break;
        case ElementType::BGRAColor:
            SetAttribute<ElementType::BGRAColor>(pAttribute, std::forward<T>(val));
            break;
        default:
            assert("Bad element type" && false);
        }
    }
protected:
    Vertex(char *pData, const VertexElementLayout &layout) noxnd;
private:
    // enables parameter pack setting of multiple parameters by element index
    template<typename First, typename ...Rest>
    void SetAttributeByIndex(size_t i, First &&first, Rest &&... rest) noxnd
    {
        SetAttributeByIndex(i, std::forward<First>(first));
        SetAttributeByIndex(i + 1, std::forward<Rest>(rest)...);
    }
    // helper to reduce code duplication in SetAttributeByIndex
    template<ElementType DestLayoutType, typename SrcType>
    void SetAttribute(char *pAttribute, SrcType &&val) noxnd
    {
        using Dest = typename VertexMap<DestLayoutType>::SysType;
        if constexpr(std::is_assignable<Dest, SrcType>::value)
        {
            *reinterpret_cast<Dest *>(pAttribute) = val;
        }
        else
        {
            assert("Parameter attribute type mismatch" && false);
        }
    }
private:
    char *pData = nullptr;
    const VertexElementLayout &vertexElementLayout;
};

class ConstVertex
{
public:
    explicit ConstVertex(const Vertex &v) noxnd;
    template<ElementType Type>
    const auto &Attr() const noxnd
    {
        return const_cast<Vertex &>(vertex).Attr<Type>();
    }
private:
    Vertex vertex;
};

class RawVertexBuffer
{
public:
    explicit RawVertexBuffer(VertexElementLayout layout, size_t size = 0ull) noxnd;
    [[nodiscard]] const char *GetData() const noxnd;
    [[nodiscard]] const VertexElementLayout &GetLayout() const noexcept;
    [[nodiscard]] size_t Size() const noxnd;
    [[nodiscard]] size_t SizeBytes() const noxnd;
    void Resize(size_t newSize) noxnd;
    template<typename ...Params>
    void EmplaceBack(Params &&... params) noxnd
    {
        assert(sizeof...(params) == vertexElementLayout.GetElements().size() && "Param count doesn't match number of vertex elements");
        buffer.resize(buffer.size() + vertexElementLayout.Size());
        Back().SetAttributeByIndex(0u, std::forward<Params>(params)...);
    }
    Vertex Back() noxnd;
    Vertex Front() noxnd;
    Vertex operator[](size_t i) noxnd;
    [[nodiscard]] ConstVertex Back() const noxnd;
    [[nodiscard]] ConstVertex Front() const noxnd;
    ConstVertex operator[](size_t i) const noxnd;
private:
    std::vector<char> buffer;
    VertexElementLayout vertexElementLayout;
};
