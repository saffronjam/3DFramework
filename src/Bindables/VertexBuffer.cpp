#include "VertexBuffer.h"

namespace Bind
{
    VertexBuffer::VertexBuffer(std::string tag, RawVertexBuffer &rawVertexBuffer)
            : m_tag(std::move(tag)),
              m_vboID(0u),
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
        const size_t vertexSize = rawVertexBuffer.GetLayout().Size();
        const auto &elements = rawVertexBuffer.GetLayout().GetElements();
        for (size_t i = 0; i < elements.size(); i++)
        {
            auto &element = elements[i];
            auto props = element.GetVertexProperties();
            auto offset = element.GetOffset();
            glVertexAttribPointer(i, props.size, props.type, props.normalized, vertexSize, (GLvoid *) (offset));
            glCheck(glEnableVertexAttribArray(i));
        }
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

    VertexBuffer::Ptr VertexBuffer::Resolve(const std::string &tag, RawVertexBuffer &rawVertexBuffer)
    {
        return Codex::Resolve<VertexBuffer>(tag, rawVertexBuffer);
    }

    std::string VertexBuffer::GetUID() const noexcept
    {
        return GenerateUID(m_tag);
    }
}