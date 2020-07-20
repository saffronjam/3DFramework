#pragma once

#include <vector>

#include "Bindable.h"
#include "VertexConfig.h"

class VertexBuffer : public Bindable
{
public:
    explicit VertexBuffer(const RawVertexBuffer &rawVertexBuffer);
    ~VertexBuffer() override;

    void BindTo(Graphics &gfx) override;
};


