#pragma once

#include <vector>
#include <memory>

#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "Bindable.h"
#include "AllBindables.h"

class Drawable
{
public:
    explicit Drawable(const glm::mat4 &baseTranslation);

    virtual void Update(const Mouse &mouse);
    void Draw(Graphics &gfx);

    void SetTranslation(const glm::vec3 &translate);
    void SetRotation(float yaw, float pitch, float roll);
    void SetProjection(const glm::mat4 &projection);

    void Translate(const glm::vec3 &translate);
    void Rotate(float yaw, float pitch, float roll);

protected:
    void AddBind(std::shared_ptr<Bindable> bindable);

protected:
    std::optional<std::shared_ptr<Bind::UniformBinder>> m_uniformBinder;

    glm::mat4 m_baseTranslation;
    glm::mat4 m_translation;
    glm::mat4 m_rotation;
    glm::mat4 m_projection;

private:
    std::optional<std::shared_ptr<Bind::IndexBuffer>> m_indexBuffer;
    std::vector<std::shared_ptr<Bindable>> m_bindables;

};


