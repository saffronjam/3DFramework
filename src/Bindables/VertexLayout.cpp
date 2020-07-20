#include "VertexLayout.h"

VertexLayout::VertexLayout(const VertexElementLayout &vertexElementLayout)
{
    glCheck(glGenVertexArrays(1, &m_GLResourceID));
    glCheck(glBindVertexArray(m_GLResourceID));

    for (auto &elements : vertexElementLayout.GetElements())
    {
        m_vertexProperties.push_back(elements.GetVertexProperties());
    }
}

VertexLayout::~VertexLayout()
{
    if (m_GLResourceID != 0)
        glCheck(glDeleteVertexArrays(1, &m_GLResourceID));
}

void VertexLayout::BindTo(Graphics &gfx)
{
    for (size_t i = 0; i < m_vertexProperties.size(); i++)
    {
        auto &prop = m_vertexProperties[i];
        glCheck(glEnableVertexAttribArray(i));
        glVertexAttribPointer(i, prop.size, prop.type, prop.normalized, prop.stride, nullptr);
        glCheck(glBindVertexArray(m_GLResourceID));
    }
}
