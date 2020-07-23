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

    auto shaderProgram = Bind::ShaderProgram::Resolve(vert, frag);
    AddBind(shaderProgram);
    AddBind(Bind::UniformBinder::Resolve(shaderProgram));
    AddBind(Bind::VertexBuffer::Resolve("TestBox", rvb));
    AddBind(Bind::IndexBuffer::Resolve("TestBox", model.indices));

    SetRotation(Random::Real(0.0f, PI<> * 2.0f), Random::Real(0.0f, PI<> * 2.0f), Random::Real(0.0f, PI<> * 2.0f));
}

void TestBox::Update(const Mouse &mouse)
{
    m_uniformBinder.value()->SetMatrix("transform", m_projection * m_translation * m_baseTranslation * m_rotation);
    m_uniformBinder.value()->SetVector("color", glm::vec3{0.5f, 0.5f, 0.5f});
}
