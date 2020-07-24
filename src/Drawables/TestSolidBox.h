#pragma once

#include "Drawable.h"
#include "Cube.h"

class TestSolidBox : public Drawable
{
public:
    TestSolidBox(const glm::mat4 &baseTranslation);

    void Update(const Mouse &mouse, const Camera &camera) override;
};
