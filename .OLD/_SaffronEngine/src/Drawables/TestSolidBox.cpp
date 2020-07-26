#include "TestSolidBox.h"

#include "Random.h"

TestSolidBox::TestSolidBox(const glm::mat4 &baseTranslation)
        : Drawable(baseTranslation)
{
    VertexElementLayout layout;
    layout.Append(ElementType::Position3D).Append(ElementType::Float3Color);

    auto model = Cube::Make(layout);
    auto &rvb = model.rawVertexBuffer;
    rvb[0].Attr<ElementType::Float3Color>() = {1.0f, 1.0f, 1.0f};
    rvb[1].Attr<ElementType::Float3Color>() = {1.0f, 1.0f, 1.0f};
    rvb[2].Attr<ElementType::Float3Color>() = {1.0f, 1.0f, 1.0f};
    rvb[3].Attr<ElementType::Float3Color>() = {1.0f, 1.0f, 1.0f};
    rvb[4].Attr<ElementType::Float3Color>() = {1.0f, 1.0f, 1.0f};
    rvb[5].Attr<ElementType::Float3Color>() = {1.0f, 1.0f, 1.0f};
    rvb[6].Attr<ElementType::Float3Color>() = {1.0f, 1.0f, 1.0f};
    rvb[7].Attr<ElementType::Float3Color>() = {1.0f, 1.0f, 1.0f};

    VertexShader vert("Shaders/SolidVS.vert");
    FragmentShader frag("Shaders/SolidFS.frag");

    auto shaderProgram = Bind::ShaderProgram::Resolve(vert, frag);
    AddBind(shaderProgram);
    AddBind(Bind::UniformBinder::Resolve(shaderProgram));
    AddBind(Bind::VertexBuffer::Resolve("TestSolidBox", rvb));
    AddBind(Bind::IndexBuffer::Resolve("TestSolidBox", model.indices));
}

void TestSolidBox::Update(const Mouse &mouse, const Camera &camera)
{
    m_uniformBinder.value()->SetMatrix("model", m_baseTranslation * m_translation * m_rotation);
    m_uniformBinder.value()->SetMatrix("view", m_view);
    m_uniformBinder.value()->SetMatrix("projection", m_projection);
}
