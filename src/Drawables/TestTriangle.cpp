#include "TestTriangle.h"

TestTriangle::TestTriangle(const glm::mat4 &baseTranslation)
        : Drawable(baseTranslation)
{
    VertexElementLayout vel;
    vel.Append(ElementType::Position3D).Append(ElementType::Float3Color);

    RawVertexBuffer rvb(std::move(vel), 3);
    rvb[0].Attr<ElementType::Position3D>() = {-0.5f, -0.5f, 0.0f};
    rvb[1].Attr<ElementType::Position3D>() = {0.5f, -0.5f, 0.0f};
    rvb[2].Attr<ElementType::Position3D>() = {0.0f, 0.5f, 0.0f};
    rvb[0].Attr<ElementType::Float3Color>() = {1.0f, 0.0f, 0.0f};
    rvb[1].Attr<ElementType::Float3Color>() = {0.0f, 1.0f, 0.0f};
    rvb[2].Attr<ElementType::Float3Color>() = {0.0f, 0.0f, 1.0f};

    std::vector<unsigned int> indices{0, 1, 2};

    VertexShader vert("Shaders/VS.vert");
    FragmentShader frag("Shaders/FS.frag");

    AddBind(Bind::ShaderProgram::Resolve(vert, frag));
    AddBind(Bind::VertexBuffer::Resolve("TestTriangle", rvb));
    AddBind(Bind::IndexBuffer::Resolve("TestTriangle", indices));

}

void TestTriangle::Update(const Mouse &mouse)
{
    m_uniformBinder.value()->SetMatrix("transform", m_projection * m_translation * m_baseTranslation * m_rotation);
}
