#include "TestTriangle.h"

TestTriangle::TestTriangle()
{
    VertexElementLayout vel;
    vel.Append(ElementType::Position3D);

    RawVertexBuffer rvb(std::move(vel), 3);
    rvb[0].Attr<ElementType::Position3D>() = {-0.5f, -0.5f, 0.0f};
    rvb[1].Attr<ElementType::Position3D>() = {0.5f, -0.5f, 0.0f};
    rvb[2].Attr<ElementType::Position3D>() = {0.0f, 0.5f, 0.0f};

    std::vector<unsigned int> indices{0, 1, 2};

    VertexShader vert("Shaders/VS.vert");
    FragmentShader frag("Shaders/FS.frag");

    auto unique_Shaderprogram = std::make_unique<ShaderProgram>(vert, frag);
    m_shaderProgram = unique_Shaderprogram.get();

    AddBind(std::move(unique_Shaderprogram));
    AddBind(std::make_unique<VertexBuffer>(rvb));
    AddBind(std::make_unique<IndexBuffer>(indices));
}

void TestTriangle::Update(const Mouse &mouse)
{
    auto pos = mouse.GetPosition();
    pos.x = pos.x / 1024.0f * 2.0f - 1.0f;
    pos.y = -1.0f * (pos.y / 720.0f * 2.0f - 1.0f);
    m_shaderProgram->SetUniform("color", glm::vec3(pos, 1.0f));
}
