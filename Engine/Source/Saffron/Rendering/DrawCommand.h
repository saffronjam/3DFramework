#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/Mesh.h"
#include "Saffron/Rendering/Material.h"

namespace Se
{
struct RenderCommand
{
	Shared<Mesh> Mesh;
	Shared<Material> Material;
	Matrix Transform;
};
}
