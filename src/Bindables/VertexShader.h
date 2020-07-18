#pragma once

#include "Shader.h"

class VertexShader : public Shader
{
public:
    explicit VertexShader(const char *shaderRaw);

    [[nodiscard]] const std::string &GetName() const noexcept override;

private:
    std::string m_name;
};