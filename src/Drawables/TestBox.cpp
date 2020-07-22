#include "TestBox.h"

#include "Random.h"

TestBox::TestBox(const glm::mat4 &baseTranslation)
        : Drawable(baseTranslation)
{
    VertexElementLayout layout;
    layout.Append(ElementType::Position3D).Append(ElementType::Float3Color);

    auto model = Cube::Make(layout);
    auto &rvb = model.rawVertexBuffer;
    rvb[0].Attr<ElementType::Float3Color>() = {1.0f, 1.0f, 1.0f};
    rvb[1].Attr<ElementType::Float3Color>() = {0.0f, 0.0f, 1.0f};
    rvb[2].Attr<ElementType::Float3Color>() = {0.0f, 1.0f, 0.0f};
    rvb[3].Attr<ElementType::Float3Color>() = {0.0f, 1.0f, 1.0f};
    rvb[4].Attr<ElementType::Float3Color>() = {1.0f, 0.0f, 0.0f};
    rvb[5].Attr<ElementType::Float3Color>() = {1.0f, 0.0f, 1.0f};
    rvb[6].Attr<ElementType::Float3Color>() = {1.0f, 1.0f, 0.0f};
    rvb[7].Attr<ElementType::Float3Color>() = {1.0f, 1.0f, 1.0f};

    VertexShader vert("Shaders/VS.vert");
    FragmentShader frag("Shaders/FS.frag");

    AddBind(std::make_shared<ShaderProgram>(vert, frag));
    AddBind(std::make_shared<VertexBuffer>(model.rawVertexBuffer));
    AddBind(std::make_shared<IndexBuffer>(model.indices));

    SetRotation(Random::Real(0.0f, PI<> * 2.0f), Random::Real(0.0f, PI<> * 2.0f), Random::Real(0.0f, PI<> * 2.0f));
}

void TestBox::Update(const Mouse &mouse)
{
    m_shaderProgram.value()->SetUniform("transform", m_projection * m_translation * m_baseTranslation * m_rotation);
}
