#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/Resources/Texture.h"

namespace Se
{
class SceneEnvironment : public Resource
{
public:
	SceneEnvironment(Path filepath, const Shared<TextureCube>& radianceMap,
	                 const Shared<TextureCube>& irradianceMap);

	const Path& GetFilepath() const;

	const Shared<TextureCube>& GetRadianceMap() const;
	const Shared<TextureCube>& GetIrradianceMap() const;

	float GetIntensity() const;
	void SetIntensity(float intensity);

	ulong GetResourceID() const override;

	static Shared<SceneEnvironment> Create(Path filepath);

private:
	Path _filePath;
	Shared<TextureCube> _radianceMap;
	Shared<TextureCube> _irradianceMap;
	float _intensity;

	static constexpr const char* SceneEnvsFolder = "SceneEnvs/";
};
}
