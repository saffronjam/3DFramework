#pragma once

#include <Saffron.h>

class SandboxLayer : public Se::Layer
{
public:
	LAYER_TYPE(Sandbox);

public:
	SandboxLayer(const Se::Window::Ptr &pWindow);

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(Se::Time ts) override;
	void OnImGuiRender() override;

	void OnEvent(const Se::Event::Ptr &pEvent) override;


};

