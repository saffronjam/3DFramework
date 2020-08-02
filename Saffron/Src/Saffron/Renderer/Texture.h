#pragma once

#include <string>

#include <Saffron/Config.h>}

namespace Se
{
class Texture
{
public:
	virtual ~Texture() = default;

	virtual Uint32 GetWidth() const = 0;
	virtual Uint32 GetHeight() const = 0;
	virtual Uint32 GetRendererID() const = 0;

	virtual void SetData(void *data, Uint32 size) = 0;

	virtual void Bind(Uint32 slot = 0) const = 0;

	virtual bool operator==(const Texture &other) const = 0;
};

class Texture2D : public Texture
{
public:
	static Ref<Texture2D> Create(Uint32 width, Uint32 height);
	static Ref<Texture2D> Create(const std::string &path);
};
}
