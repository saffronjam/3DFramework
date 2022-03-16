#pragma once

#include "Saffron/Core/TypeDefs.h"

namespace Se
{
enum class ModelTextureMapType
{
	Albedo = 0,
	Normal = 1,
	Roughness = 2,
	Metalness = 3,
	Count
};


using ShaderBindFlags = uint;

enum ShaderBindFlags_
{
	BindFlag_VS = 1 << 0,
	BindFlag_PS = 1 << 1,
	BindFlag_CS = 1 << 2,
	BindFlag_GS = 1 << 3
};


using TextureUsage = uint;

enum TextureUsageFlags : uint
{
	TextureUsage_UnorderedAccess = 1 << 0,
	TextureUsage_ShaderResource = 1 << 1
};
}
