#pragma once

#include <vector>

#include "Bindable.h"

class VertexBuffer : public Bindable
{
public:
    template<class Vertex>
    explicit VertexBuffer(const std::vector<Vertex> &vertices)
    {
        glGenBuffers(1, &m_GLResourceID);
        glBindBuffer(GL_ARRAY_BUFFER, m_GLResourceID);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    }
    ~VertexBuffer() override;

    void BindTo(Graphics &gfx) override;
};


