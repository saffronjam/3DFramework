#pragma once

#include <Saffron.h>

#include "EditorLayer.h"
#include "StartupLayer.h"

class EditorApplication : public Application
{
public:
	explicit EditorApplication(const Properties &props);

	void OnInit() override;
	void OnUpdate() override;

private:
	Shared<StartupLayer> m_StartupLayer;
	Shared<EditorLayer> m_EditorLayer;
};
