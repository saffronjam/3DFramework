#pragma once

#include "Saffron/Scene/Scene.h"

namespace Se
{
class RuntimeScene : public Scene
{
public:
	explicit RuntimeScene(Shared<Scene> copyFrom);

	void OnUpdate() override;
	void OnRender() override;
	void OnGuiRender() override;
	void OnStart();
	void OnStop();

	void Escape();
	void Return();

	const Shared<EditorCamera>& GetDebugCamera() const { return _debugCamera; }

private:
	Shared<EditorCamera> _debugCamera;
	bool _debugCameraActivated = false;
};
}
