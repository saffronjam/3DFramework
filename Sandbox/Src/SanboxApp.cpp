#include <Saffron.h>

class SandboxApp : public Saffron::Application
{
public:
	SandboxApp()
	{
	}

	~SandboxApp()
	{
	}
};

Saffron::Application::Ptr Saffron::CreateApplication()
{
	return std::make_shared<Saffron::Application>();
}