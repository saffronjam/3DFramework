#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/Enums.h"
#include "Saffron/Rendering/GraphicsResource.h"

namespace Se
{
class Uniform : public GraphicsResource
{
public:
	using Ptr = Shared<Uniform>;

public:
	Uniform(String name, UniformType::Enum type, Uint16 arraySize = 1);
	virtual ~Uniform();

	template<typename UniformDataType>
	void Set(const UniformDataType& data, Uint32 arraySize = 1);
	
	bgfx::UniformHandle GetNativeHandle();

	static Ptr Create(String name, UniformType::Enum type, Uint16 arraySize = 1);

protected:
	String _name;

	bgfx::UniformHandle _handle;
};

template <typename UniformDataType>
void Uniform::Set(const UniformDataType& data, Uint32 arraySize)
{
	bgfx::setUniform(_handle, data, arraySize);
}
}
