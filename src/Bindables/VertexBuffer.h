#pragma once

#include <vector>

#include "Bindable.h"

class VertexBuffer : public Bindable
{
public:
    template<class Vertex>
    explicit VertexBuffer(const std::vector<Vertex> &vertices)
            : m_VBO(0u)
    {
        glGenBuffers(1, &m_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    }

    void BindTo(Graphics &gfx) override;

private:
    unsigned int m_VBO;
};


