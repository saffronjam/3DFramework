#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/Mesh.h"
#include "Saffron/Rendering/Material.h"

namespace Se
{
struct DrawCommand
{
	DrawCommand(Shared<Mesh> mesh, Shared<Material> material, Matrix transform) :
		Mesh(mesh),
		Material(material),
		Transform(transform)
	{
	}

	Shared<Mesh> Mesh;
	Shared<Material> Material;
	Matrix Transform;
};
}
