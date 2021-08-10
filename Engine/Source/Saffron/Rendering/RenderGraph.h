#pragma once

#include <memory>

#include "Saffron/Base.h"
#include "Saffron/Rendering/RenderPass.h"

namespace Se
{
class RenderGraph
{
public:
	void Setup(SceneCommon& sceneCommon);
	void Execute();

	void SetViewportSize(uint width, uint height);

	void RegisterInput(const std::string& name, std::shared_ptr<Framebuffer>& framebuffer);
	void RegisterOutput(const std::string& name, const std::shared_ptr<Framebuffer>& framebuffer);

private:
	void LinkGlobalInput(const std::string& input, const std::string& provider);

protected:
	void AddPass(std::unique_ptr<RenderPass> pass);

private:
	void Connect();
	void ConnectInputs(std::map<std::string, Input>& inputs);

private:
	std::vector<std::unique_ptr<RenderPass>> _passes;
	std::map<std::string, Input> _globalInputs;
	std::map<std::string, Output> _globalOutputs;
};

namespace Utils
{
std::vector<std::string> SplitString(const std::string& input, const std::string& delim);
}
}
