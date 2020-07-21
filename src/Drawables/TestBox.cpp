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
    AddBind(std::make_unique<IndexBuffer>(model.indices));
}

void TestBox::Update(const Mouse &mouse)
{
    auto pos = mouse.GetPosition();
    pos.x = pos.x / 1024.0f * 2.0f - 1.0f;
    pos.y = -1.0f * (pos.y / 720.0f * 2.0f - 1.0f);
    m_shaderProgram->SetUniform("color", glm::vec3(pos, 1.0f));
}
