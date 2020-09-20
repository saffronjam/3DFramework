#include <Saffron.h>
#include <Saffron/EntryPoint.h>

#include "EditorLayer.h"

class SaffronBunApplication : public Se::Application
{
public:
	SaffronBunApplication(const Properties &props)
		: Application(props)
	{
	}

	virtual void OnInit() override
	{
		PushLayer(new Se::EditorLayer());
	}
};

Se::Application *Se::CreateApplication()
{
	return new SaffronBunApplication({ "Hazelnut", 1600, 900 });
}
