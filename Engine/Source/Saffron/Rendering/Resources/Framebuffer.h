#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/Resource.h"
#include "Saffron/Rendering/RendererAPI.h"

namespace Se
{
enum class FramebufferTextureFormat
{
	None = 0,

	// Color
	RGBA8 = 1,
	RGBA16F = 2,
	RGBA32F = 3,
	RG32F = 4,

	// Depth/stencil
	DEPTH32F = 5,
	DEPTH24STENCIL8 = 6,

	// Defaults
	Depth = DEPTH24STENCIL8
};

struct FramebufferTextureSpecification
{
	FramebufferTextureSpecification() = default;

	FramebufferTextureSpecification(FramebufferTextureFormat format) :
		TextureFormat(format)
	{
	}

	FramebufferTextureFormat TextureFormat;
	// TODO: filtering/wrap
};

struct FramebufferAttachmentSpecification
{
	FramebufferAttachmentSpecification() = default;

	FramebufferAttachmentSpecification(const std::initializer_list<FramebufferTextureSpecification>& attachments) :
		Attachments(attachments)
	{
	}

	std::vector<FramebufferTextureSpecification> Attachments;
};

struct FramebufferSpecification
{
	uint32_t Width = 1280;
	uint32_t Height = 720;
	glm::vec4 ClearColor;
	FramebufferAttachmentSpecification Attachments;
	uint32_t Samples = 1; // multisampling

	// TODO: Temp, needs scale
	bool NoResize = false;

	// SwapChainTarget = screen buffer (i.e. no framebuffer)
	bool SwapChainTarget = false;
};

class Framebuffer : public Resource
{
public:
	virtual ~Framebuffer()
	{
	}

	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;

	virtual void Resize(uint32_t width, uint32_t height, bool forceRecreate = false) = 0;

	virtual void BindTexture(uint32_t attachmentIndex = 0, uint32_t slot = 0) const = 0;

	virtual uint32_t GetWidth() const = 0;
	virtual uint32_t GetHeight() const = 0;

	virtual RendererID GetRendererID() const = 0;
	virtual RendererID GetColorAttachmentRendererID(int index = 0) const = 0;
	virtual RendererID GetDepthAttachmentRendererID() const = 0;

	virtual const FramebufferSpecification& GetSpecification() const = 0;

	static Shared<Framebuffer> Create(const FramebufferSpecification& spec);
};

class FramebufferPool final : public SingleTon<FramebufferPool>
{
public:
	FramebufferPool(uint32_t maxFBs = 32);
	~FramebufferPool();

	std::weak_ptr<Framebuffer> AllocateBuffer();
	void Add(const Shared<Framebuffer>& framebuffer);

	std::vector<Shared<Framebuffer>>& GetAll() { return m_Pool; }

	const std::vector<Shared<Framebuffer>>& GetAll() const { return m_Pool; }

	static FramebufferPool* GetGlobal() { return s_Instance; }

private:
	std::vector<Shared<Framebuffer>> m_Pool;

	static FramebufferPool* s_Instance;
};
}
