#pragma once

#include <Saffron.h>

#include "Layers/EditorLayer.h"

namespace Se
{
class EditorApp : public App
{
public:
	EditorApp(const AppSpec& spec);
	
	void OnCreate() override;

	void OnExit() override;

private:
	std::shared_ptr<EditorLayer> _editorLayer;
	
};
}
