#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(const RawVertexBuffer &rawVertexBuffer)
{
    glCheck(glGenBuffers(1, &m_GLResourceID));
    glCheck(glBindBuffer(GL_ARRAY_BUFFER, m_GLResourceID));
    glCheck(glBufferData(GL_ARRAY_BUFFER, rawVertexBuffer.SizeBytes(), rawVertexBuffer.GetData(), GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer()
{
    if (m_GLResourceID != 0u)
        glCheck(glDeleteBuffers(1, &m_GLResourceID));
}

void VertexBuffer::BindTo(Graphics &gfx)
{
    glCheck(glBindBuffer(GL_ARRAY_BUFFER, m_GLResourceID));
}

