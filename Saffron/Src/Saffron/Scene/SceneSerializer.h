#pragma once



#include "Saffron/Scene/Scene.h"

namespace Se
{
class SceneSerializer
{
public:
	SceneSerializer(const Shared<Scene> &scene);

	void Serialize(const Filepath &filepath);
	void SerializeRuntime(const Filepath &filepath);

	bool Deserialize(const Filepath &filepath);
	bool DeserializeRuntime(const Filepath &filepath);
private:
	Shared<Scene> m_Scene;
};
}

