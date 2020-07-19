#pragma once

#include "Bindable.h"

class IndexBuffer : public Bindable
{
public:
    explicit IndexBuffer(std::vector<unsigned int> indices);
    ~IndexBuffer() override;

    void BindTo(Graphics &gfx) override;

private:
    std::vector<unsigned int> m_indices;

};


