#pragma once

#include "Shader.h"

class FragmentShader : public Shader
{
public:
    explicit FragmentShader(const char *shaderRaw);

    [[nodiscard]] const std::string &GetName() const noexcept override;

private:
    std::string m_name;
};