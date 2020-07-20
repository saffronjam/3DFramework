#pragma once

#include "Bindable.h"

class IndexBuffer : public Bindable
{
public:
    explicit IndexBuffer(std::vector<unsigned int> indices);
    ~IndexBuffer();

    [[nodiscard]] const std::vector<unsigned int> &GetIndices() const noexcept;

    void BindTo(Graphics &gfx) override;

private:
    unsigned int m_iboID;
    std::vector<unsigned int> m_indices;
};


