#include "Texture.h"

#include <GL/glew.h>

#include "BindableCodex.h"

namespace Bind
{
    Texture::Texture(const Surface &surface)
            : m_filepath(surface.GetFilepath()),
              m_textureID(0u)
    {
        glCheck(glGenTextures(1, &m_textureID));
        glCheck (glBindTexture(GL_TEXTURE_2D, m_textureID));
        glCheck (glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, surface.GetWidth(), surface.GetHeight(), 0, GL_RGBA, GL_FLOAT, surface.GetBufferPtr()));
        glCheck (glGenerateMipmap(GL_TEXTURE_2D));

        // Set the texture wrapping/filtering options (on the currently bound texture object)
        glCheck (glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
        glCheck (glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        glCheck (glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    }

    Texture::Texture(const std::string &filepath)
            : Texture(Surface::FromFile(filepath))
    {
    }

    void Texture::BindTo(Graphics &gfx)
    {
        glCheck(glBindTexture(GL_TEXTURE_2D, m_textureID));
    }

    Texture::Ptr Texture::Resolve(const Surface &surface)
    {
        return Codex::Resolve<Texture>(surface);
    }

    Texture::Ptr Texture::Resolve(const std::string &filepath)
    {
        return Codex::Resolve<Texture>(filepath);
    }

    std::string Texture::GetUID() const noexcept
    {
        return GenerateUID(m_filepath);
    }
}