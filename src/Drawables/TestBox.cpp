#include "TestBox.h"

TestBox::TestBox()
        : m_shaderProgram(nullptr)
{
    auto model = Cube::Make();

    VertexShader vert("Shaders/VS.vert");
    FragmentShader frag("Shaders/FS.frag");

    auto unique_Shaderprogram = std::make_unique<ShaderProgram>(vert, frag);
    m_shaderProgram = unique_Shaderprogram.get();

    AddBind(std::move(unique_Shaderprogram));
    AddBind(std::make_unique<VertexBuffer>(model.rawVertexBuffer));

}

void TestBox::Update(const Mouse &mouse)
{
    Drawable::Update(mouse);
}
