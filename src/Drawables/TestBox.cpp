#include "TestBox.h"

TestBox::TestBox()
        : m_shaderProgram(nullptr),
          m_transform(1)
{
    VertexElementLayout layout;
    layout.Append(ElementType::Position3D).Append(ElementType::Float3Color);

    auto model = Cube::Make(layout);
    auto &rvb = model.rawVertexBuffer;
    rvb[0].Attr<ElementType::Float3Color>() = {0.0f, 1.0f, 1.0f};
    rvb[1].Attr<ElementType::Float3Color>() = {0.0f, 1.0f, 1.0f};
    rvb[2].Attr<ElementType::Float3Color>() = {0.0f, 1.0f, 1.0f};
    rvb[3].Attr<ElementType::Float3Color>() = {0.0f, 1.0f, 1.0f};
    rvb[4].Attr<ElementType::Float3Color>() = {0.0f, 1.0f, 1.0f};
    rvb[5].Attr<ElementType::Float3Color>() = {0.0f, 1.0f, 1.0f};
    rvb[6].Attr<ElementType::Float3Color>() = {0.0f, 1.0f, 1.0f};
    rvb[7].Attr<ElementType::Float3Color>() = {0.0f, 1.0f, 1.0f};

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
    m_shaderProgram->SetUniform("transform", m_transform);
}

void TestBox::Rotate(float yaw, float pitch, float roll)
{
    m_transform *= glm::yawPitchRoll(yaw, pitch, roll);
}
