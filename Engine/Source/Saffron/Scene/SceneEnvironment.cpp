#include "SaffronPCH.h"

#include "Saffron/Rendering/SceneRenderer.h"
#include "Saffron/Scene/SceneEnvironment.h"
#include "Saffron/Resource/ResourceManager.h"

namespace Se
{
SceneEnvironment::SceneEnvironment(Path filepath, const Shared<TextureCube>& radianceMap,
                                   const Shared<TextureCube>& irradianceMap) :
	_filePath(Move(filepath)),
	_radianceMap(radianceMap),
	_irradianceMap(irradianceMap),
	_intensity(1.0f)
{
}

const Path& SceneEnvironment::GetFilepath() const
{
	return _filePath;
}

const Shared<TextureCube>& SceneEnvironment::GetRadianceMap() const
{
	return _radianceMap;
}

const Shared<TextureCube>& SceneEnvironment::GetIrradianceMap() const
{
	return _irradianceMap;
}

float SceneEnvironment::GetIntensity() const
{
	return _intensity;
}

void SceneEnvironment::SetIntensity(float intensity)
{
	_intensity = intensity;
}

ulong SceneEnvironment::GetResourceID() const
{
	return HashFilepath(_filePath);
}

Shared<SceneEnvironment> SceneEnvironment::Create(Path filepath)
{
	const Path fullFilepath = SceneEnvsFolder + Move(filepath).string();

	const ulong filepathHash = HashFilepath(fullFilepath);
	if (ResourceManager::Contains(filepathHash))
	{
		return ResourceManager::Get(filepathHash);
	}

	Shared<SceneEnvironment> newSceneEnvironment = SceneRenderer::CreateEnvironmentMap(fullFilepath);
	ResourceManager::Add(newSceneEnvironment);
	return newSceneEnvironment;
}
}
