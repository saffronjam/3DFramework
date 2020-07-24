#include "TestBox.h"

#include "Random.h"

TestBox::TestBox(const glm::mat4 &baseTranslation)
        : Drawable(baseTranslation)
{
    auto model = Cube::MakeIndependentTextured();
    auto &rvb = model.rawVertexBuffer;

    VertexShader vert("Shaders/VS.vert");
    FragmentShader frag("Shaders/FS.frag");

    auto shaderProgram = Bind::ShaderProgram::Resolve(vert, frag);
    AddBind(shaderProgram);
    AddBind(Bind::UniformBinder::Resolve(shaderProgram));
    AddBind(Bind::VertexBuffer::Resolve("TestBox", rvb));
    AddBind(Bind::IndexBuffer::Resolve("TestBox", model.indices));
    AddBind(Bind::Texture::Resolve("Images/sample_image.png"));

    SetRotation(Random::Real(0.0f, PI<> * 2.0f), Random::Real(0.0f, PI<> * 2.0f), Random::Real(0.0f, PI<> * 2.0f));
}

void TestBox::Update(const Mouse &mouse)
{
    m_uniformBinder.value()->SetMatrix("model", m_baseTranslation * m_translation * m_rotation);
    m_uniformBinder.value()->SetMatrix("view", m_view);
    m_uniformBinder.value()->SetMatrix("projection", m_projection);
    m_uniformBinder.value()->SetVector("extraColor", m_extraColor);
}
