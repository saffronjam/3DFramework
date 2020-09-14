#pragma once

#include "Saffron/Renderer/RendererAPI.h"
#include "Saffron/System/SaffronMath.h"

namespace Se
{
class Framebuffer : public RefCounted
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
		uint32_t Width = 1280;
		uint32_t Height = 720;
		glm::vec4 ClearColor;
		Format Format;
		uint32_t Samples = 1; // multisampling

		// SwapChainTarget = screen buffer (i.e. no framebuffer)
		bool SwapChainTarget = false;
	};

public:
	virtual ~Framebuffer() = default;
	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;

	virtual void Resize(uint32_t width, uint32_t height, bool forceRecreate = false) = 0;

	virtual void BindTexture(uint32_t slot = 0) const = 0;

	virtual RendererID GetRendererID() const = 0;
	virtual RendererID GetColorAttachmentRendererID() const = 0;
	virtual RendererID GetDepthAttachmentRendererID() const = 0;

	virtual const Specification &GetSpecification() const = 0;

	static Ref<Framebuffer> Create(const Specification &spec);
};

class FramebufferPool final
{
public:
	std::weak_ptr<Framebuffer> AllocateBuffer();
	void Add(const Ref<Framebuffer> &framebuffer);

	std::vector<Ref<Framebuffer>> &GetAll() { return m_Pool; }
	const std::vector<Ref<Framebuffer>> &GetAll() const { return m_Pool; }

	static FramebufferPool *GetGlobal() { return sInstance; }
private:
	std::vector<Ref<Framebuffer>> m_Pool;

	static FramebufferPool *sInstance;
};
}

