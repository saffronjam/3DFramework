#include "TestTriangle.h"

TestTriangle::TestTriangle()
{
    struct Vertex
    {
        glm::vec3 pos;
    };

    std::vector<Vertex> vertices = {
            {{-0.5f, -0.5f, 0.0f}},
            {{0.5f,  -0.5f, 0.0f}},
            {{0.0f,  0.5f,  0.0f}}
    };

    VertexShader vert("Shaders/VS.vert");
    FragmentShader frag("Shaders/FS.frag");

    VertexShader vert2 = std::move(vert);
    FragmentShader frag2 = std::move(frag);

    ShaderProgram shaderProgram(vert2, frag2);
    shaderProgram.SetUniform("color", glm::vec3(0.5f, 0.5f, 0.9f));

    AddBind(std::make_unique<ShaderProgram>(std::move(shaderProgram)));
    AddBind(std::make_unique<VertexBuffer>(vertices));
    AddBind(std::make_unique<VertexLayout>());

}
