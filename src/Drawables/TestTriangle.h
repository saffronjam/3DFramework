#pragma once

#include "Drawable.h"

class TestTriangle : public Drawable
{
public:
    TestTriangle(const glm::mat4 &baseTranslation);

    void Update(const Mouse &mouse) override;
};


