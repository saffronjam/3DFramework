#include "VertexLayout.h"

VertexLayout::VertexLayout()
{
    glCheck(glGenVertexArrays(1, &m_GLResourceID));
    glCheck(glBindVertexArray(m_GLResourceID));

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

}

VertexLayout::~VertexLayout()
{
    if (m_GLResourceID != 0)
        glCheck(glDeleteVertexArrays(1, &m_GLResourceID));
}

void VertexLayout::BindTo(Graphics &gfx)
{
    glCheck(glEnableVertexAttribArray(0));
    glCheck(glBindVertexArray(m_GLResourceID));
}
