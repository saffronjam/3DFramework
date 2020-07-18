#pragma once

#include "Bindable.h"
#include "ShaderProgram.h"

class Shader : public Bindable, ShaderProgram
{
public:
    Shader();
    ~Shader();
    void Compile(const char *shaderRaw, GLenum shaderType);

    void BindTo(Graphics &gfx) override;

protected:
    [[nodiscard]] virtual const std::string &GetName() const noexcept = 0;

private:
    unsigned int m_shaderID;
};


