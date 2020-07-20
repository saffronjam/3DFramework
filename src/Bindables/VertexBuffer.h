#pragma once

#include <vector>

#include "Bindable.h"
#include "VertexConfig.h"

class VertexBuffer : public Bindable
{
public:
    explicit VertexBuffer(const RawVertexBuffer &rawVertexBuffer);
    ~VertexBuffer();

    void BindTo(Graphics &gfx) override;

private:
    unsigned int m_vboID;
    unsigned int m_vaoID;
};


