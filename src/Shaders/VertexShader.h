#pragma once

#include "Shader.h"

class VertexShader : public Shader
{
public:
    explicit VertexShader(const char *shaderRaw);
};