#pragma once

#include "Saffron/Core/Math/SaffronMath.h"
#include "Saffron/Renderer/RendererAPI.h"
#include "Saffron/Resource/Resource.h"

namespace Se
{
class Framebuffer : public Resource
{
public:
	enum class TextureFormat
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

	struct TextureSpecification
	{
		TextureSpecification() = default;
		TextureSpecification(TextureFormat format) : TextureFormat(format) {}

		TextureFormat TextureFormat;
		// TODO: filtering/wrap
	};

	struct AttachmentSpecification
	{
		AttachmentSpecification() = default;
		AttachmentSpecification(const InitializerList<TextureSpecification> &attachments)
			: Attachments(attachments)
		{
		}

		ArrayList<TextureSpecification> Attachments;
	};

	struct Specification
	{
		Uint32 Width = 1280;
		Uint32 Height = 720;
		Vector4f ClearColor;
		AttachmentSpecification Attachments;
		Uint32 Samples = 1; // multisampling
		// 
		// TODO: Temp, needs scale
		bool NoResize = false;

		// SwapChainTarget = screen buffer (i.e. no framebuffer)
		bool SwapChainTarget = false;
	};

public:
	virtual ~Framebuffer() = default;
	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;

	virtual void Resize(Uint32 width, Uint32 height, bool forceRecreate = false) = 0;

	virtual void BindTexture(Uint32 attachmentIndex = 0, Uint32 slot = 0) const = 0;

	virtual Uint32 GetWidth() const = 0;
	virtual Uint32 GetHeight() const = 0;

	virtual RendererID GetRendererID() const = 0;
	virtual RendererID GetColorAttachmentRendererID(int index = 0) const = 0;
	virtual RendererID GetDepthAttachmentRendererID() const = 0;

	virtual const Specification &GetSpecification() const = 0;

	static std::shared_ptr<Framebuffer> Create(const Specification &spec);
};

class FramebufferPool final
{
public:
	std::weak_ptr<Framebuffer> AllocateBuffer();
	void Add(const std::shared_ptr<Framebuffer> &framebuffer);
	void Clear() { m_Pool.clear(); }

	ArrayList<std::shared_ptr<Framebuffer>> &GetAll() { return m_Pool; }
	const ArrayList<std::shared_ptr<Framebuffer>> &GetAll() const { return m_Pool; }

	static FramebufferPool *GetGlobal() { return sInstance; }

private:
	ArrayList<std::shared_ptr<Framebuffer>> m_Pool;
	static FramebufferPool *sInstance;
};
}

