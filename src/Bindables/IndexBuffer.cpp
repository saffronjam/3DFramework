#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(std::vector<unsigned int> indices)
        : m_indices(std::move(indices))
{
    // Generate a buffer for the indices
    GLuint elementbuffer;
    glCheck(glGenBuffers(1, &elementbuffer));
    glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer));
    glCheck(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
    if (m_GLResourceID != 0)
        glCheck(glDeleteBuffers(1, &m_GLResourceID));
}

void IndexBuffer::BindTo(Graphics &gfx)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_GLResourceID);
}
