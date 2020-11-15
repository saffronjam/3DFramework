#pragma once

#include <Saffron.h>

#include "EditorLayer.h"
#include "StartupLayer.h"

class SaffronBunApplication : public Application
{
public:
	explicit SaffronBunApplication(const Properties &props);

	void OnInit() override;
	void OnUpdate() override;

private:
	Shared<StartupLayer> m_StartupLayer;
	Shared<EditorLayer> m_EditorLayer;
};
