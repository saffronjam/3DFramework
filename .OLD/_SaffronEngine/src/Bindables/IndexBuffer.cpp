#include "IndexBuffer.h"

#include <GL/glew.h>

#include "BindableCodex.h"

namespace Bind
{
    IndexBuffer::IndexBuffer(const std::string &tag)
            : IndexBuffer(tag, {})
    {
    }

    IndexBuffer::IndexBuffer(std::string tag, IndexVector indices)
            : m_tag(std::move(tag)),
              m_iboID(0u),
              m_indices(std::move(indices))
    {
        glCheck(glGenBuffers(1, &m_iboID));
        glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboID));
        glCheck(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW));
    }

    IndexBuffer::~IndexBuffer()
    {
        if (m_iboID != 0)
            glCheck(glDeleteBuffers(1, &m_iboID));
    }

    const IndexVector &IndexBuffer::GetIndices() const noexcept
    {
        return m_indices;
    }

    void IndexBuffer::BindTo(Graphics &gfx)
    {
        glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboID));
    }

    IndexBuffer::Ptr IndexBuffer::Resolve(const std::string &tag, const IndexVector &indices)
    {
        return Codex::Resolve<IndexBuffer>(tag, indices);
    }

    void IndexBuffer::SetIndices(IndexVector indices) noexcept
    {
        m_indices = std::move(indices);
    }

    std::string IndexBuffer::GetUID() const noexcept
    {
        return IndexBuffer::GenerateUID(m_tag);
    }
}