#include "TestBox.h"

TestBox::TestBox()
{
    auto model = Cube::Make();
    
}

void TestBox::Update(const Mouse &mouse)
{
    Drawable::Update(mouse);
}
