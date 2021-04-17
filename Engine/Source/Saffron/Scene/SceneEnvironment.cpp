#include "SaffronPCH.h"

#include "Saffron/Rendering/SceneRenderer.h"
#include "Saffron/Scene/SceneEnvironment.h"

namespace Se
{
SceneEnvironment::SceneEnvironment(Filepath filepath, const Shared<TextureCube>& radianceMap,
                                   const Shared<TextureCube>& irradianceMap) :
	_filePath(Move(filepath)),
	_radianceMap(radianceMap),
	_irradianceMap(irradianceMap),
	_intensity(1.0f)
{
}

Shared<SceneEnvironment> SceneEnvironment::Load(Filepath filepath)
{
	const Filepath fullFilepath = SceneEnvsFolder + Move(filepath).string();
	Shared<SceneEnvironment> newSceneEnvironment = SceneRenderer::CreateEnvironmentMap(fullFilepath);
	return newSceneEnvironment;
}
}
