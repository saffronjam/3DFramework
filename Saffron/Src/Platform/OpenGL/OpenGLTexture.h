#pragma once

#include "Saffron/Config.h"
#include "Saffron/Renderer/Texture.h"

namespace Se
{
class OpenGLTexture2D : public Texture2D
{
public:
	OpenGLTexture2D(Uint32 width, Uint32 height);
	OpenGLTexture2D(const std::string &path);
	virtual ~OpenGLTexture2D();

	Uint32 GetWidth() const override { return m_Width; }
	Uint32 GetHeight() const override { return m_Height; }
	Uint32 GetRendererID() const override { return m_RendererID; }

	void SetData(void *data, Uint32 size) override;

	void Bind(Uint32 slot = 0) const override;

	bool operator==(const Texture &other) const override
	{
		return m_RendererID == (dynamic_cast<const OpenGLTexture2D &>(other)).m_RendererID;
	}
private:
	std::string m_Path;
	Uint32 m_Width, m_Height;
	Uint32 m_RendererID;
	Uint32 m_InternalFormat, m_DataFormat;
};
}

