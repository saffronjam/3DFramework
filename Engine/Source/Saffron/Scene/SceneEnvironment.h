#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/Resources/Texture.h"

namespace Se
{
class SceneEnvironment : public Managed
{
public:
	SceneEnvironment(Filepath filepath, const Shared<TextureCube>& radianceMap,
	                 const Shared<TextureCube>& irradianceMap);

	const Filepath& GetFilepath() const { return _filePath; }

	const Shared<TextureCube>& GetRadianceMap() const { return _radianceMap; }

	const Shared<TextureCube>& GetIrradianceMap() const { return _irradianceMap; }

	float GetIntensity() const { return _intensity; }

	void SetIntensity(float intensity) { _intensity = intensity; }

	static Shared<SceneEnvironment> Load(Filepath filepath);

private:
	Filepath _filePath;
	Shared<TextureCube> _radianceMap;
	Shared<TextureCube> _irradianceMap;
	float _intensity;

	static constexpr const char* SceneEnvsFolder = "SceneEnvs/";
};
}
