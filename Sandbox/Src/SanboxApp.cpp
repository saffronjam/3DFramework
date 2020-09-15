#include <Saffron.h>
#include <Saffron/EntryPoint.h>

#include "EditorLayer.h"

namespace Se
{
class SandboxApp : public Application
{
public:
	SandboxApp(const Properties &props = { "Sandbox App", 1024, 720 })
		: Application(props)
	{
	}

	void OnInit() override
	{
		PushLayer(new EditorLayer(m_Keyboard, m_Mouse));
	}

private:
	Ref<EditorLayer> m_layer;
};

Ref<Application> CreateApplication()
{
	return Ref<SandboxApp>::Create();
}
}
