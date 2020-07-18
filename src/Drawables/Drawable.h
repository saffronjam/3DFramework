#pragma once

#include <vector>
#include <memory>

#include "Bindable.h"
#include "AllBindables.h"

class Drawable
{
public:
    void Draw(Graphics &gfx);

protected:
    void AddBind(std::unique_ptr<Bindable> bindable);

private:
    std::vector<std::unique_ptr<Bindable>> m_bindables;
};


