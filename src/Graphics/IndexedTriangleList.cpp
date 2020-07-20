#include "IndexedTriangleList.h"

IndexedTriangleList::IndexedTriangleList(Dvtx::VertexBuffer verts_in, std::vector<unsigned short> indices_in)
        :
        vertices(std::move(verts_in)),
        indices(std::move(indices_in))
{
    assert(vertices.Size() > 2);
    assert(indices.size() % 3 == 0);
}

void IndexedTriangleList::Transform(const glm::mat4 &matrix)
{
    for (int i = 0; i < vertices.Size(); i++)
    {
        auto &pos = vertices[i].Attr<Dvtx::ElementType::Position3D>();
        pos = matrix * glm::vec4(pos, 1.0f);
    }
}

void IndexedTriangleList::SetNormalsIndependentFlat() noxnd
{
    for (size_t i = 0; i < indices.size(); i += 3)
    {
        auto v0 = vertices[indices[i]];
        auto v1 = vertices[indices[i + 1]];
        auto v2 = vertices[indices[i + 2]];
        const auto &p0 = v0.Attr<Dvtx::ElementType::Position3D>();
        const auto &p1 = v1.Attr<Dvtx::ElementType::Position3D>();
        const auto &p2 = v2.Attr<Dvtx::ElementType::Position3D>();

        const auto n = glm::normalize(glm::cross(p1 - p0, p2 - p0));

        v0.Attr<Dvtx::ElementType::Normal>() = n;
        v1.Attr<Dvtx::ElementType::Normal>() = n;
        v2.Attr<Dvtx::ElementType::Normal>() = n;
    }
}

