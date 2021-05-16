#pragma once

#include "Saffron/Base.h"
#include "Saffron/Scene/Scene.h"

namespace YAML
{
class Emitter;
}

namespace Se
{
class SceneSerializer
{
public:
	explicit SceneSerializer(Scene& scene);

	void Serialize(const Path& filepath) const;
	void SerializeRuntime(const Path& filepath) const;

	bool Deserialize(const Path& filepath);
	bool DeserializeRuntime(const Path& filepath);

private:
	void SerializeEntity(YAML::Emitter& emitter, Entity entity) const;
	void SerializeEnvironment(YAML::Emitter& emitter) const;
	void SerializeEditorCamera(YAML::Emitter& emitter) const;


private:
	Scene& _scene;
};
}
