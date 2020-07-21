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
    Drawable();

    virtual void Update(const Mouse &mouse);
    void Draw(Graphics &gfx);

    void SetTranslation(const glm::vec3& translate);
    void SetRotation(float yaw, float pitch, float roll);
    void SetProjection(const glm::mat4 &projection);

    void Translate(const glm::vec3& translate);
    void Rotate(float yaw, float pitch, float roll);

protected:
    void AddBind(std::unique_ptr<Bindable> bindable);

protected:
    glm::mat4 m_translation;
    glm::mat4 m_rotation;
    glm::mat4 m_projection;

private:
    std::vector<std::unique_ptr<Bindable>> m_bindables;

    std::optional<IndexBuffer *> m_indexBuffer;
};


