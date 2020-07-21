#pragma once

#include <vector>

#include "VertexConfig.h"
#include "SeMath.h"

class IndexedTriangleList
{
public:
    IndexedTriangleList(RawVertexBuffer rawVertexBuffer, std::vector<unsigned int> indices);
    void Transform(const glm::mat4 &matrix);
    void SetNormalsIndependentFlat() noxnd;

public:
    RawVertexBuffer rawVertexBuffer;
    std::vector<unsigned int> indices;
};