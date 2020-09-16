#include <Saffron.h>
#include <Saffron/EntryPoint.h>

#include "EditorLayer.h"

class SandboxApp : public Se::Application
{
public:
	SandboxApp(const Properties &props = { "Sandbox App", 1024, 720 })
		: Application(props)
	{
	}

	void OnInit() override
	{
		PushLayer(new Se::EditorLayer(m_Keyboard, m_Mouse));
	}
};

Se::Application *Se::CreateApplication()
{
	return new SandboxApp();
}
