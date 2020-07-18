#include "Drawable.h"

void Drawable::Draw(Graphics &gfx)
{
    for (auto &bindable : m_bindables)
    {
        bindable->BindTo(gfx);
    }
    gfx.DrawArrays();
}

void Drawable::AddBind(std::unique_ptr<Bindable> bindable)
{
    m_bindables.push_back(std::move(bindable));
}
