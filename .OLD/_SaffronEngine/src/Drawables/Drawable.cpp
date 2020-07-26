#include "Drawable.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

Drawable::Drawable(const glm::mat4 &baseTranslation)
        : m_baseTranslation(baseTranslation),
          m_translation(1),
          m_rotation(1),
          m_projection(1)
{
}

void Drawable::Update(const Mouse &mouse, const Camera &camera)
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

void Drawable::AddBind(std::shared_ptr<Bindable> bindable)
{
    if (dynamic_cast<Bind::IndexBuffer *>(bindable.get()))
    {
        m_indexBuffer = std::make_optional(std::static_pointer_cast<Bind::IndexBuffer>(bindable));
    }
    if (dynamic_cast<Bind::UniformBinder *>(bindable.get()))
    {
        m_uniformBinder = std::make_optional(std::static_pointer_cast<Bind::UniformBinder>(bindable));
    }
    m_bindables.push_back(std::move(bindable));
}

void Drawable::SetView(const glm::mat4 &view) noexcept
{
    m_view = view;
}

void Drawable::SetTranslation(const glm::vec3 &translate) noexcept
{
    m_translation = glm::translate(translate);
}

void Drawable::SetRotation(float yaw, float pitch, float roll) noexcept
{
    m_rotation = glm::yawPitchRoll(yaw, pitch, roll);
}

void Drawable::SetProjection(const glm::mat4 &projection) noexcept
{
    m_projection = projection;
}

void Drawable::SetExtraValues(const glm::vec3 &values) noexcept
{
    m_extraValues = values;
}

void Drawable::Translate(const glm::vec3 &translate) noexcept
{
    m_translation *= glm::translate(translate);
}

void Drawable::Rotate(float yaw, float pitch, float roll) noexcept
{
    m_rotation *= glm::yawPitchRoll(yaw, pitch, roll);
}
