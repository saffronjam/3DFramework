#pragma once

#include "Saffron/Core/Math/SaffronMath.h"
#include "Saffron/Renderer/RendererAPI.h"

namespace Se
{
class Framebuffer : public ReferenceCounted
{
public:
	enum class Format
	{
		None = 0,
		RGBA8 = 1,
		RGBA16F = 2
	};

	struct Specification
	{
		Uint32 Width = 1280;
		Uint32 Height = 720;
		glm::vec4 ClearColor;
		Format Format;
		Uint32 Samples = 1; // multisampling

		// SwapChainTarget = screen buffer (i.e. no framebuffer)
		bool SwapChainTarget = false;
	};

public:
	virtual ~Framebuffer() = default;
	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;

	virtual void Resize(Uint32 width, Uint32 height, bool forceRecreate = false) = 0;

	virtual void BindTexture(Uint32 slot = 0) const = 0;

	virtual RendererID GetRendererID() const = 0;
	virtual RendererID GetColorAttachmentRendererID() const = 0;
	virtual RendererID GetDepthAttachmentRendererID() const = 0;

	virtual const Specification &GetSpecification() const = 0;

	static Shared<Framebuffer> Create(const Specification &spec);
};

class FramebufferPool final
{
public:
	std::weak_ptr<Framebuffer> AllocateBuffer();
	void Add(const Shared<Framebuffer> &framebuffer);

	std::vector<Shared<Framebuffer>> &GetAll() { return m_Pool; }
	const std::vector<Shared<Framebuffer>> &GetAll() const { return m_Pool; }

	static FramebufferPool *GetGlobal() { return sInstance; }
private:
	std::vector<Shared<Framebuffer>> m_Pool;

	static FramebufferPool *sInstance;
};
}

