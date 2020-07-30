#pragma once

#include <Saffron.h>

class SandboxLayer : public Se::Layer
{
public:
	SandboxLayer();

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(Se::Time ts) override;
	void OnImGuiRender() override;

	void OnEvent(const Se::Event &event) override;


};
