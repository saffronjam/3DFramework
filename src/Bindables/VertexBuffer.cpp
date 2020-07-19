#include "VertexBuffer.h"

VertexBuffer::~VertexBuffer()
{
    if(m_GLResourceID != 0u)
        glCheck(glDeleteBuffers(1, &m_GLResourceID));
}

void VertexBuffer::BindTo(Graphics &gfx)
{
    glCheck(glBindBuffer(GL_ARRAY_BUFFER, m_GLResourceID));
}