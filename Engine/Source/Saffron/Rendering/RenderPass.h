#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/Resources/Framebuffer.h"
#include "Saffron/Rendering/RenderGraph.h"

namespace Se
{
struct RenderPassSpecification
{
	Shared<Framebuffer> TargetFramebuffer;
};

class RenderPass : public Managed
{
public:
	explicit RenderPass(String name);

	bool operator ==(const RenderPass& rhs) const;

	virtual void Execute() = 0;
	virtual void OnViewportResize(Uint32 width, Uint32 height) = 0;

	const String& GetName() const;
	void SetStreamLinkage(const String& outputName, const String& inputName);

	const UnorderedMap<String, Unique<Stream::Input>>& GetInputs() const;
	const UnorderedMap<String, Unique<Stream::Output>>& GetOutputs() const;
	const Stream::Input& GetInput(const String& name) const;
	const Stream::Output& GetOutput(const String& name) const;

protected:
	void AddInput(Unique<Stream::Input> input);
	void AddOutput(Unique<Stream::Output> output);

private:
	Stream::Input& GetInput(const String& name);
	Stream::Output& GetOutput(const String& name);

private:
	UUID _uuid;
	String _name;

	UnorderedMap<String, Unique<Stream::Input>> _inputs;
	UnorderedMap<String, Unique<Stream::Output>> _outputs;
};
}
