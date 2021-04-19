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
	FramebufferTextureSpecification(FramebufferTextureFormat format);

	FramebufferTextureFormat TextureFormat;
	// TODO: filtering/wrap
};

struct FramebufferAttachmentSpecification
{
	FramebufferAttachmentSpecification() = default;
	FramebufferAttachmentSpecification(const InitializerList<FramebufferTextureSpecification>& attachments);

	ArrayList<FramebufferTextureSpecification> Attachments;
};

struct FramebufferSpecification
{
	Uint32 Width = 1280;
	Uint32 Height = 720;
	Vector4f ClearColor;
	FramebufferAttachmentSpecification Attachments;
	Uint32 Samples = 1; // multisampling

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

	virtual void Clear() = 0;

	virtual void Resize(Uint32 width, Uint32 height, bool forceRecreate = false) = 0;

	virtual void BindTexture(Uint32 attachmentIndex = 0, Uint32 slot = 0) const = 0;

	virtual Uint32 GetWidth() const = 0;
	virtual Uint32 GetHeight() const = 0;

	virtual RendererID GetRendererID() const = 0;
	virtual RendererID GetColorAttachmentRendererID(int index = 0) const = 0;
	virtual RendererID GetDepthAttachmentRendererID() const = 0;

	virtual const FramebufferSpecification& GetSpecification() const = 0;

	static Shared<Framebuffer> Create(const FramebufferSpecification& spec);
};

class FramebufferPool final : public SingleTon<FramebufferPool>
{
public:
	explicit FramebufferPool(Uint32 maxFBs = 32);
	~FramebufferPool();

	Weak<Framebuffer> AllocateBuffer();
	void Add(const Shared<Framebuffer>& framebuffer);

	ArrayList<Shared<Framebuffer>>& GetAll() { return _pool; }

	const ArrayList<Shared<Framebuffer>>& GetAll() const { return _pool; }

	static FramebufferPool* GetGlobal() { return s_Instance; }

private:
	ArrayList<Shared<Framebuffer>> _pool;

	static FramebufferPool* s_Instance;
};
}
