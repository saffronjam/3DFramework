#pragma once

#include <memory>

#include "Saffron/Common/Enums.h"

namespace Se
{
template <class ShaderBased>
class ShaderBindFlagsBinder
{
public:
	explicit ShaderBindFlagsBinder(const std::shared_ptr<ShaderBased>& shaderBased, ShaderBindFlags flags) :
		_shaderBased(shaderBased),
		_oldFlags(_shaderBased->BindFlags())
	{
		_shaderBased->SetBindFlags(flags);
	}

	~ShaderBindFlagsBinder()
	{
		_shaderBased->SetUsage(_oldFlags);
	}

private:
	const std::shared_ptr<ShaderBased>& _shaderBased;
	const ShaderBindFlags _oldFlags;
};

#define ScopedBindFlagsBinderLine2(shaderBased, flags, line) ShaderBindFlagsBinder binder##line(shaderBased, flags)
#define ScopedBindFlagsBinderLine(shaderBased, flags, line) ScopedBindFlagsBinderLine2(shaderBased, flags, line)
#define ScopedBindFlagsBinder(shaderBased, flags) ScopedBindFlagsBinderLine(shaderBased, flags, __LINE__)

}
