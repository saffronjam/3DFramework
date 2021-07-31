#pragma once

#include <map>

#include "Saffron/Base.h"
#include "Saffron/Rendering/Stream/Input.h"
#include "Saffron/Rendering/Stream/Output.h"

namespace Se
{
class RenderPass
{
public:
	explicit RenderPass(std::string name);
	virtual ~RenderPass() = default;

	virtual void Execute() = 0;

	void Connect(const Output& output, const std::string& inside);
	void Connect(const std::shared_ptr<Framebuffer>& output, const std::string& inside);

	auto Name() const -> const std::string&;
	auto Inputs() const -> const std::map<std::string, Input>&;
	auto Outputs() const -> const std::map<std::string, Output>&;

protected:
	void RegisterInput(const std::string& name, std::shared_ptr<Framebuffer>& framebuffer);
	void RegisterOutput(const std::string& name, const std::shared_ptr<Framebuffer>& framebuffer);

private:
	std::string _name;
	std::map<std::string, Input> _inputs;
	std::map<std::string, Output> _outputs;
};
}
