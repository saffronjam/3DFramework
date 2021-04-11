#include "SaffronPCH.h"

#include "Saffron/Rendering/Passes/GeometryPass.h"

#include "Saffron/Rendering/Renderer.h"

namespace Se
{
GeometryPass::GeometryPass() :
	_geoProgram(CreateShared<Program>("cubes"))
{
}

void GeometryPass::Execute(const RenderGraph& rendergraph)
{
	const auto& activeScene = rendergraph.GetActiveScene().GetCameraTransforms();
	Renderer::Instance().SetViewProjection(activeScene.MainView, activeScene.MainProjection);

	const auto& commands = rendergraph.GetDrawCommands(RenderChannel::Main);
	for (auto& command : commands)
	{
		Renderer::Instance().Submit(command.Mesh, _geoProgram, command.Material);
	}
}
}
