#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(const RawVertexBuffer &rawVertexBuffer)
        : m_vboID(0u),
          m_vaoID(0u)
{
    // Generate VAO
    glCheck(glGenVertexArrays(1, &m_vaoID));
    glCheck(glBindVertexArray(m_vaoID));

    // Generate VBO and copy vertex data
    glCheck(glGenBuffers(1, &m_vboID));
    glCheck(glBindBuffer(GL_ARRAY_BUFFER, m_vboID));
    glCheck(glBufferData(GL_ARRAY_BUFFER, rawVertexBuffer.SizeBytes(), rawVertexBuffer.GetData(), GL_STATIC_DRAW));

    // Configure Vertex Attrib pointer which is stored in bound VAO
    auto prop = rawVertexBuffer.GetLayout().GetElements().front().GetVertexProperties();
    glCheck(glVertexAttribPointer(0, prop.size, prop.type, prop.normalized, prop.stride, nullptr));
    glCheck(glEnableVertexAttribArray(0));
}

VertexBuffer::~VertexBuffer()
{
    if (m_vboID != 0u)
        glCheck(glDeleteBuffers(1, &m_vboID));
    if (m_vaoID != 0)
        glCheck(glDeleteVertexArrays(1, &m_vaoID));
}

void VertexBuffer::BindTo(Graphics &gfx)
{
    glCheck(glBindBuffer(GL_ARRAY_BUFFER, m_vboID));
    glCheck(glBindVertexArray(m_vaoID));
}

