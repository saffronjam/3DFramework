#include <Saffron.h>
#include <Saffron/EntryPoint.h>

#include "SandboxLayer.h"

class SandboxApp : public Se::Application
{
public:
	SandboxApp(const Se::Application::Properties &props)
		: Application(props)
	{
	}

	virtual void OnInit() override
	{
		PushLayer(new Hazel::EditorLayer());
	}

private:
	Se::Ref<SandboxLayer> m_layer;
};

Se::Ref<Se::Application> Se::CreateApplication()
{
	return Se::CreateRef<SandboxApp>();
}
