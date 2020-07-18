#include "VertexBuffer.h"

void VertexBuffer::BindTo(Graphics &gfx)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
}
