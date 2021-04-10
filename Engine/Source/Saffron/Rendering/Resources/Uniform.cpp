#include "SaffronPCH.h"

#include "Saffron/Rendering/Resources/Uniform.h"

namespace Se
{
Uniform::Uniform(String name, UniformType::Enum type, Uint16 arraySize) :
	_name(Move(name)),
	_handle(bgfx::createUniform(name.c_str(), static_cast<bgfx::UniformType::Enum>(type), arraySize))
{
}

Uniform::~Uniform()
{
	bgfx::destroy(_handle);
}

bgfx::UniformHandle Uniform::GetNativeHandle()
{
	return _handle;
}

Uniform::Ptr Uniform::Create(String name, UniformType::Enum type, Uint16 arraySize)
{
	return CreateShared<Uniform>(Move(name), type, arraySize);
}
}
