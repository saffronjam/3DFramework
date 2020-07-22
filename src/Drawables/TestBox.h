#pragma once

#include "Drawable.h"
#include "Cube.h"

class TestBox : public Drawable
{
public:
    TestBox(const glm::mat4 &baseTranslation);

    void Update(const Mouse &mouse) override;
};
