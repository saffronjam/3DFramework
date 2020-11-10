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
		PushLayer(new StartupLayer());
	}
};

Application *Se::CreateApplication()
{
	return new SaffronBunApplication({ "Saffron Bun", 1600, 900 });
}
