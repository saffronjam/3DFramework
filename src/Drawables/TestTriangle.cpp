#include "TestTriangle.h"

#include <glm/vec3.hpp>

TestTriangle::TestTriangle()
{
    const char *vertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                     "}\0";

    const char* fragmentShaderSource = "#version 330 core\n"
                                       "out vec4 FragColor;\n"
                                       "void main()\n"
                                       "{\n"
                                       "    FragColor = vec4(1.0f, 0.3f, 0.2f, 1.0f);\n"
                                       "}\0";
    struct Vertex
    {
        glm::vec3 pos;
    };

    std::vector<Vertex> vertices = {
            {{-0.5f, -0.5f, 0.0f}},
            {{0.5f, -0.5f, 0.0f}},
            {{0.0f, 0.5f, 0.0f}}
    };

    AddBind(std::make_unique<VertexBuffer>(vertices));
    AddBind(std::make_unique<VertexShader>(vertexShaderSource));
    AddBind(std::make_unique<FragmentShader>(fragmentShaderSource));
    AddBind(std::make_unique<VertexLayout>());
}
