#pragma once

#include <map>

#include "Saffron/Base.h"
#include "Saffron/Rendering/Stream/Input.h"
#include "Saffron/Rendering/Stream/Output.h"
#include "Saffron/Ui/Ui.h"

namespace Se
{
struct SceneCommon;

class RenderPass
{
public:
	explicit RenderPass(std::string name, struct SceneCommon& sceneCommon);
	virtual ~RenderPass() = default;

	virtual void OnSetupFinished()
	{
	}

	virtual void OnUi()
	{
	}

	virtual void Execute() = 0;

	void Connect(const Output& output, const std::string& inside);
	void Connect(const std::shared_ptr<Framebuffer>& output, const std::string& inside);

	auto Name() const -> const std::string&;
	auto Inputs() -> std::map<std::string, Input>&;
	auto Inputs() const -> const std::map<std::string, Input>&;
	auto Outputs() -> std::map<std::string, Output>&;
	auto Outputs() const -> const std::map<std::string, Output>&;

	void LinkInput(const std::string& input, const std::string& provider);

	virtual void SetViewportSize(uint width, uint height)
	{
	}

protected:
	void RegisterInput(const std::string& name, std::shared_ptr<Framebuffer>& framebuffer);
	void RegisterOutput(const std::string& name, const std::shared_ptr<Framebuffer>& framebuffer);

protected:
	auto SceneCommon() -> struct SceneCommon&;
	auto SceneCommon() const -> const struct SceneCommon&;

private:
	std::string _name;
	struct SceneCommon& _sceneCommon;
	std::map<std::string, Input> _inputs;
	std::map<std::string, Output> _outputs;
};
}
