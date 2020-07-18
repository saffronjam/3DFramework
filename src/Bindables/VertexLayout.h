#pragma once

#include "Bindable.h"

class VertexLayout : public Bindable
{
public:
    VertexLayout();

    void BindTo(Graphics &gfx) override;

private:
    unsigned int m_VAO;
};


