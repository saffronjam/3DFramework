#pragma once

#include <vector>
#include <memory>

#include "Bindable.h"
#include "AllBindables.h"

class Drawable
{
public:
    virtual void Update(const Mouse &mouse);
    void Draw(Graphics &gfx);

protected:
    void AddBind(std::unique_ptr<Bindable> bindable);

private:
    std::vector<std::unique_ptr<Bindable>> m_bindables;

    std::optional<IndexBuffer *> m_indexBuffer;
};


