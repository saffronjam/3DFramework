#include "FragmentShader.h"

FragmentShader::FragmentShader(const char *shaderRaw)
        : m_name("Fragment shader")
{
    Compile(shaderRaw, GL_FRAGMENT_SHADER);
}

const std::string &FragmentShader::GetName() const noexcept
{
    return m_name;
}