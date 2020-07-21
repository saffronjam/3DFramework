#include "Drawable.h"

Drawable::Drawable()
        : m_translation(glm::translate(glm::vec3(0.0f, 0.0f, -5.0f))),
          m_rotation(1),
          m_projection(1)
{
}

void Drawable::Update(const Mouse &mouse)
{

}

void Drawable::Draw(Graphics &gfx)
{
    for (auto &bindable : m_bindables)
    {
        bindable->BindTo(gfx);
    }
    if (m_indexBuffer.has_value())
    {
        gfx.DrawIndexed(m_indexBuffer.value()->GetIndices());
    }
    else
    {
        gfx.DrawArrays();
    }
}

void Drawable::AddBind(std::unique_ptr<Bindable> bindable)
{
    if (dynamic_cast<IndexBuffer *>(bindable.get()))
    {
        m_indexBuffer = dynamic_cast<IndexBuffer * >(bindable.get());
    }
    m_bindables.push_back(std::move(bindable));
}

void Drawable::SetTranslation(const glm::vec3 &translate)
{
     m_translation = glm::translate(translate);
}

void Drawable::SetRotation(float yaw, float pitch, float roll)
{
    m_rotation = glm::yawPitchRoll(yaw, pitch, roll);
}

void Drawable::SetProjection(const glm::mat4 &projection)
{
    m_projection = projection;
}

void Drawable::Translate(const glm::vec3 &translate)
{
    m_translation *= glm::translate(translate);
}

void Drawable::Rotate(float yaw, float pitch, float roll)
{
    m_rotation *= glm::yawPitchRoll(yaw, pitch, roll);
}