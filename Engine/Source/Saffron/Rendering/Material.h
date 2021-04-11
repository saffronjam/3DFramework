#pragma once

#include "Saffron/Base.h"

namespace Se
{
class Material
{
public:
	Color Ambient;
	Color Diffuse;
	Color Specular;

	Material(const Color& ambient, const Color& diffuse, const Color& specular) :
		Ambient(ambient),
		Diffuse(diffuse),
		Specular(specular)
	{
	}
};
}
