#pragma once

#include "Bindable.h"
#include "VertexConfig.h"

class VertexLayout : public Bindable
{
public:
    explicit VertexLayout(const VertexElementLayout &vertexElementLayout);
    ~VertexLayout() override;

    void BindTo(Graphics &gfx) override;

private:
    std::vector<VertexProperties> m_vertexProperties;
};


