#pragma once

#include <vector>
#include <memory>

#include "SeMath.h"
#include "Bindable.h"
#include "AllBindables.h"
#include "Camera.h"

class Drawable
{
public:
    explicit Drawable(const glm::mat4 &baseTranslation);

    virtual void Update(const Mouse &mouse, const Camera &camera);
    void Draw(Graphics &gfx);

    void SetView(const glm::mat4 &view) noexcept;
    void SetTranslation(const glm::vec3 &translate) noexcept;
    void SetRotation(float yaw, float pitch, float roll) noexcept;
    void SetProjection(const glm::mat4 &projection) noexcept;
    void SetExtraValues(const glm::vec3 &values) noexcept;

    //tmp
    glm::vec3 lightPos = {0.0f, 0.0f, 0.0f};
    glm::vec3 lightColor = {1.0f, 1.0f, 1.0f};
    float ambientIntensity = 0.3f;
    float diffuseIntensity = 0.3f;
    float specularIntensity = 0.3f;
    float specularPower = 512.0f;
    float attConst;
    float attLin;
    float attQuad;

    void Translate(const glm::vec3 &translate) noexcept;
    void Rotate(float yaw, float pitch, float roll) noexcept;

protected:
    void AddBind(std::shared_ptr<Bindable> bindable);

protected:
    std::optional<std::shared_ptr<Bind::UniformBinder>> m_uniformBinder;

    glm::mat4 m_baseTranslation;
    glm::mat4 m_translation;
    glm::mat4 m_rotation;
    glm::mat4 m_projection;
    glm::mat4 m_view;
    glm::vec3 m_extraValues;

private:
    std::optional<std::shared_ptr<Bind::IndexBuffer>> m_indexBuffer;
    std::vector<std::shared_ptr<Bindable>> m_bindables;

};


