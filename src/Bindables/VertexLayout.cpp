#include "VertexLayout.h"

VertexLayout::VertexLayout()
        : m_VAO(0u)
{
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);

}
void VertexLayout::BindTo(Graphics &gfx)
{
    glEnableVertexAttribArray(0);
    glBindVertexArray(m_VAO);
}
