#include "Drawable.h"

void Drawable::Update(const Mouse &mouse)
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

void Drawable::AddBind(std::unique_ptr<Bindable> bindable)
{
    if (dynamic_cast<IndexBuffer*>(bindable.get()))
    {
        m_indexBuffer = dynamic_cast<IndexBuffer * >(bindable.get());
    }
    m_bindables.push_back(std::move(bindable));
}
