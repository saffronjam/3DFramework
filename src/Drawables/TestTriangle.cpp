#include "TestTriangle.h"

TestTriangle::TestTriangle()
{
    const char *vertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "uniform vec3 color;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   gl_Position = vec4(aPos.x + color.r, aPos.y + color.g, aPos.z +  + color.b, 1.0);\n"
                                     "}\0";

    const char *fragmentShaderSource = "#version 330 core\n"
                                       "out vec4 FragColor;\n"
                                       "uniform vec3 color;\n"
                                       "void main()\n"
                                       "{\n"
                                       "    FragColor = vec4(color, 1.0f);\n"
                                       "}\0";
    struct Vertex
    {
        glm::vec3 pos;
    };

    std::vector<Vertex> vertices = {
            {{-0.5f, -0.5f, 0.0f}},
            {{0.5f,  -0.5f, 0.0f}},
            {{0.0f,  0.5f,  0.0f}}
    };

    VertexShader vert(vertexShaderSource);
    FragmentShader frag(fragmentShaderSource);

    ShaderProgram tmp(vert, frag);
    tmp.SetUniform("color", glm::vec3(0.5f, 0.5f, 0.9f));

    AddBind(std::make_unique<ShaderProgram>(std::move(tmp)));
    AddBind(std::make_unique<VertexBuffer>(vertices));
    AddBind(std::make_unique<VertexLayout>());

}
