#pragma once

#include <memory>

#include "Saffron/Common/Enums.h"

namespace Se
{
template <class TextureClass>
class TextureUsageBinder
{
public:
	explicit TextureUsageBinder(const std::shared_ptr<TextureClass>& texture, TextureUsage flags) :
		_texture(texture),
		_oldFlags(_texture->Usage())
	{
		_texture->SetUsage(flags);
	}

	~TextureUsageBinder()
	{
		_texture->SetUsage(_oldFlags);
	}

private:
	const std::shared_ptr<TextureClass>& _texture;
	const TextureUsage _oldFlags;
};

#define ScopedTexUsageBinderLine2(shaderBased, flags, line) TextureUsageBinder binder##line(shaderBased, flags)
#define ScopedTexUsageBinderLine(shaderBased, flags, line) ScopedTexUsageBinderLine2(shaderBased, flags, line)
#define ScopedTexUsageBinder(shaderBased, flags) ScopedTexUsageBinderLine(shaderBased, flags, __LINE__)

}
