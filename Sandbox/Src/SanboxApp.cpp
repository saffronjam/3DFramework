#include <Saffron.h>
#include <Saffron/EntryPoint.h>

#include "SandboxLayer.h"

class SandboxApp : public Se::Application
{
public:
	SandboxApp()
		:
		Application(),
		m_layer(Se::CreateRef<SandboxLayer>(m_Keyboard, m_Mouse))
	{
		PushLayer(m_layer);
	}

	~SandboxApp()
	{
	}

private:
	Se::Ref<SandboxLayer> m_layer;
};

Se::Ref<Se::Application> Se::CreateApplication()
{
	return Se::CreateRef<SandboxApp>();
}
