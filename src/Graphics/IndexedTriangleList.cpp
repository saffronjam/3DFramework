#include "IndexedTriangleList.h"

IndexedTriangleList::IndexedTriangleList(RawVertexBuffer rawVertexBuffer, std::vector<unsigned int> indices)
        : rawVertexBuffer(std::move(rawVertexBuffer)),
          indices(std::move(indices))
{
    assert(this->rawVertexBuffer.Size() > 2);
    assert(this->indices.size() % 3 == 0);
}

void IndexedTriangleList::Transform(const glm::mat4 &matrix)
{
    for (int i = 0; i < rawVertexBuffer.Size(); i++)
    {
        auto &pos = rawVertexBuffer[i].Attr<ElementType::Position3D>();
        pos = matrix * glm::vec4(pos, 1.0f);
    }
}

void IndexedTriangleList::SetNormalsIndependentFlat() noxnd
{
    for (size_t i = 0; i < indices.size(); i += 3)
    {
        auto v0 = rawVertexBuffer[indices[i]];
        auto v1 = rawVertexBuffer[indices[i + 1]];
        auto v2 = rawVertexBuffer[indices[i + 2]];
        const auto &p0 = v0.Attr<ElementType::Position3D>();
        const auto &p1 = v1.Attr<ElementType::Position3D>();
        const auto &p2 = v2.Attr<ElementType::Position3D>();

        const auto n = glm::normalize(glm::cross(p1 - p0, p2 - p0));

        v0.Attr<ElementType::Normal>() = n;
        v1.Attr<ElementType::Normal>() = n;
        v2.Attr<ElementType::Normal>() = n;
    }
}

