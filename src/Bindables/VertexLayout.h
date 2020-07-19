#pragma once

#include "Bindable.h"

class VertexLayout : public Bindable
{
public:
    VertexLayout();
    ~VertexLayout();

    void BindTo(Graphics &gfx) override;
};


