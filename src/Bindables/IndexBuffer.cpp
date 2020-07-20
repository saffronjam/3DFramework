#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(std::vector<unsigned int> indices)
        : m_iboID(0u),
          m_indices(std::move(indices))
{
    glCheck(glGenBuffers(1, &m_iboID));
    glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboID));
    glCheck(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
    if (m_iboID != 0)
        glCheck(glDeleteBuffers(1, &m_iboID));
}

void IndexBuffer::BindTo(Graphics &gfx)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboID);
}
const std::vector<unsigned int> &IndexBuffer::GetIndices() const noexcept
{
    return m_indices;
}
