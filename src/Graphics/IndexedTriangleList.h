#pragma once

#include <vector>

#include "Vertex.h"
#include "SeMath.h"

class IndexedTriangleList
{
public:
    IndexedTriangleList(Dvtx::VertexBuffer verts_in, std::vector<unsigned short> indices_in);
    void Transform(const glm::mat4 &matrix);
    void SetNormalsIndependentFlat() noxnd;

public:
    Dvtx::VertexBuffer vertices;
    std::vector<unsigned short> indices;
};