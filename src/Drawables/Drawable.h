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
    virtual void Update(const Mouse &mouse);
    void Draw(Graphics &gfx);

    virtual void Rotate(float yaw, float pitch, float roll) = 0;

protected:
    void AddBind(std::unique_ptr<Bindable> bindable);

private:
    std::vector<std::unique_ptr<Bindable>> m_bindables;

    std::optional<IndexBuffer *> m_indexBuffer;
};


