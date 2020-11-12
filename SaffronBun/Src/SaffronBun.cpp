#define SAFFRON_ENTRY_POINT
#include <Saffron.h>

#include "EditorLayer.h"
#include "StartupLayer.h"

class SaffronBunApplication : public Application
{
public:
	SaffronBunApplication(const Properties &props)
		: Application(props)
	{
	}

	void OnInit() override
	{
		auto *startupLayer = new StartupLayer();
		startupLayer->SetOnProjectSelectCallback([](const Project &project)
												 {
													 // PopLayer
												 });
		PushLayer(startupLayer);
	}
};

Application *Se::CreateApplication()
{
	return new SaffronBunApplication({ "Saffron Bun", 1600, 900 });
}
