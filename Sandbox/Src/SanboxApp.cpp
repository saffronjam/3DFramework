#include <Saffron.h>
#include <Saffron/EntryPoint.h>

#include "SandboxLayer.h"

class SandboxApp : public Se::Application
{
public:
	SandboxApp()
		: m_layer(Se::CreateRef<SandboxLayer>(GetWindow()))
	{
		PushLayer(m_layer);
	}

	~SandboxApp()
	{
	}

private:
	SandboxLayer::Ptr m_layer;
};

Se::Application::Ptr Se::CreateApplication()
{
	return Se::CreateRef<SandboxApp>();
}
