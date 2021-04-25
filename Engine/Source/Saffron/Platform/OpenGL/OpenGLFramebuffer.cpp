#include "SaffronPCH.h"

#include <glad/glad.h>

#include "Saffron/Platform/OpenGL/OpenGLFramebuffer.h"
#include "Saffron/Rendering/Renderer.h"


namespace Se
{
namespace Utils
{
static GLenum TextureTarget(bool multisampled)
{
	return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
}

static void CreateTextures(bool multisampled, RendererID* outID, uint count)
{
	glCreateTextures(TextureTarget(multisampled), 1, outID);
}

static void BindTexture(bool multisampled, RendererID id)
{
	glBindTexture(TextureTarget(multisampled), id);
}

static GLenum DataType(GLenum format)
{
	switch (format)
	{
	case GL_RGBA8: return GL_UNSIGNED_BYTE;
	case GL_RG16F:
	case GL_RG32F:
	case GL_RGBA16F:
	case GL_RGBA32F: return GL_FLOAT;
	case GL_DEPTH24_STENCIL8: return GL_UNSIGNED_INT_24_8;
	}

	Debug::Assert(false, "Unknown format!");;
	return 0;
}

static void AttachColorTexture(RendererID id, int samples, GLenum format, uint width, uint height, int index)
{
	bool multisampled = samples > 1;
	if (multisampled)
	{
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
	}
	else
	{
		// Only RGBA access for now
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, DataType(format), nullptr);

		glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
}

static void AttachDepthTexture(RendererID id, int samples, GLenum format, GLenum attachmentType, uint width,
                               uint height)
{
	bool multisampled = samples > 1;
	if (multisampled)
	{
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
	}
	else
	{
		glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

		glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(TextureTarget(multisampled), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
}

static bool IsDepthFormat(FramebufferTextureFormat format)
{
	switch (format)
	{
	case FramebufferTextureFormat::DEPTH24STENCIL8:
	case FramebufferTextureFormat::DEPTH32F: return true;
	}
	return false;
}
}

OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& specification) :
	_specification(specification),
	_width(specification.Width),
	_height(specification.Height)
{
	for (auto format : _specification.Attachments.Attachments)
	{
		if (!Utils::IsDepthFormat(format.TextureFormat)) _colorAttachmentFormats.emplace_back(format.TextureFormat);
		else _depthAttachmentFormat = format.TextureFormat;
	}

	OpenGLFramebuffer::Resize(specification.Width, specification.Height, true);
}

OpenGLFramebuffer::~OpenGLFramebuffer()
{
	Shared<OpenGLFramebuffer> instance = this;
	Renderer::Submit([instance]()
	{
		glDeleteFramebuffers(1, &instance->_rendererID);
	});
}

void OpenGLFramebuffer::Resize(uint width, uint height, bool forceRecreate)
{
	if (!forceRecreate && (_width == width && _height == height)) return;

	_width = width;
	_height = height;

	Shared<OpenGLFramebuffer> instance = this;
	Renderer::Submit([instance]() mutable
	{
		if (instance->_rendererID)
		{
			glDeleteFramebuffers(1, &instance->_rendererID);
			glDeleteTextures(instance->_colorAttachments.size(), instance->_colorAttachments.data());
			glDeleteTextures(1, &instance->_depthAttachment);

			instance->_colorAttachments.clear();
			instance->_depthAttachment = 0;
		}

		glGenFramebuffers(1, &instance->_rendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, instance->_rendererID);

		bool multisample = instance->_specification.Samples > 1;

		if (instance->_colorAttachmentFormats.size())
		{
			instance->_colorAttachments.resize(instance->_colorAttachmentFormats.size());
			Utils::CreateTextures(multisample, instance->_colorAttachments.data(), instance->_colorAttachments.size());

			// Create color attachments
			for (int i = 0; i < instance->_colorAttachments.size(); i++)
			{
				Utils::BindTexture(multisample, instance->_colorAttachments[i]);
				switch (instance->_colorAttachmentFormats[i])
				{
				case FramebufferTextureFormat::RGBA8: Utils::AttachColorTexture(
						instance->_colorAttachments[i], instance->_specification.Samples, GL_RGBA8, instance->_width,
						instance->_height, i);
					break;
				case FramebufferTextureFormat::RGBA16F: Utils::AttachColorTexture(
						instance->_colorAttachments[i], instance->_specification.Samples, GL_RGBA16F, instance->_width,
						instance->_height, i);
					break;
				case FramebufferTextureFormat::RGBA32F: Utils::AttachColorTexture(
						instance->_colorAttachments[i], instance->_specification.Samples, GL_RGBA32F, instance->_width,
						instance->_height, i);
					break;
				case FramebufferTextureFormat::RG32F: Utils::AttachColorTexture(
						instance->_colorAttachments[i], instance->_specification.Samples, GL_RG32F, instance->_width,
						instance->_height, i);
					break;
				}
			}
		}

		if (instance->_depthAttachmentFormat != FramebufferTextureFormat::None)
		{
			Utils::CreateTextures(multisample, &instance->_depthAttachment, 1);
			Utils::BindTexture(multisample, instance->_depthAttachment);
			switch (instance->_depthAttachmentFormat)
			{
			case FramebufferTextureFormat::DEPTH24STENCIL8: Utils::AttachDepthTexture(
					instance->_depthAttachment, instance->_specification.Samples, GL_DEPTH24_STENCIL8,
					GL_DEPTH_STENCIL_ATTACHMENT, instance->_width, instance->_height);
				break;
			case FramebufferTextureFormat::DEPTH32F: Utils::AttachDepthTexture(
					instance->_depthAttachment, instance->_specification.Samples, GL_DEPTH_COMPONENT32F,
					GL_DEPTH_ATTACHMENT, instance->_width, instance->_height);
				break;
			}
		}

		if (instance->_colorAttachments.size() > 1)
		{
			Debug::Assert(instance->_colorAttachments.size() <= 4);;
			GLenum buffers[4] = {
				GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3
			};
			glDrawBuffers(instance->_colorAttachments.size(), buffers);
		}
		else if (instance->_colorAttachments.size() == 0)
		{
			// Only depth-pass
			glDrawBuffer(GL_NONE);
		}

#if 0

			if (multisample)
			{
				glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &_colorAttachment);
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _colorAttachment);

				// TODO: Create Hazel texture object based on format here
				if (_specification.Format == FramebufferFormat::RGBA16F)
				{
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, _specification.Samples, GL_RGBA16F, _specification.Width, _specification.Height, GL_FALSE);
				}
				else if (_specification.Format == FramebufferFormat::RGBA8)
				{
					glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, _specification.Samples, GL_RGBA8, _specification.Width, _specification.Height, GL_FALSE);
				}
				// glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				// glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
			}
			else
			{
				glCreateTextures(GL_TEXTURE_2D, 1, &_colorAttachment);
				glBindTexture(GL_TEXTURE_2D, _colorAttachment);

				// TODO: Create Hazel texture object based on format here
				if (_specification.Format == FramebufferFormat::RGBA16F)
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _specification.Width, _specification.Height, 0, GL_RGBA, GL_FLOAT, nullptr);
				}
				else if (_specification.Format == FramebufferFormat::RG32F) // "Shadow" for now
				{
					glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, _specification.Width, _specification.Height);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _colorAttachment, 0);
					glDrawBuffer(GL_NONE);
				}
				else if (_specification.Format == FramebufferFormat::RGBA8)
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _specification.Width, _specification.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
				}
				else if (_specification.Format == FramebufferFormat::COMP)
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _specification.Width, _specification.Height, 0, GL_RGBA, GL_FLOAT, nullptr);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

					glCreateTextures(GL_TEXTURE_2D, 1, &_colorAttachment2);
					glBindTexture(GL_TEXTURE_2D, _colorAttachment2);
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _specification.Width, _specification.Height, 0, GL_RGBA, GL_FLOAT, nullptr);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				}

				if (_specification.Format != FramebufferFormat::RG32F)
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorAttachment, 0);
				}
			}

			if (multisample)
			{
				glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &_depthAttachment);
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _depthAttachment);
				glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, _specification.Samples, GL_DEPTH24_STENCIL8, _specification.Width, _specification.Height, GL_FALSE);
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
			}
			else if (_specification.Format != FramebufferFormat::RG32F)
			{
				glCreateTextures(GL_TEXTURE_2D, 1, &_depthAttachment);
				glBindTexture(GL_TEXTURE_2D, _depthAttachment);
				glTexImage2D(
					GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, _specification.Width, _specification.Height, 0,
					GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
				);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, _depthAttachment, 0);
			}

			if (_specification.Format != FramebufferFormat::RG32F)
			{
				if (multisample)
				{
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, _colorAttachment, 0);
				}
				else
				{
					glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _colorAttachment, 0);
					if (_specification.Format == FramebufferFormat::COMP)
					{
						glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, _colorAttachment2, 0);
						const GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
						glDrawBuffers(2, buffers);
					}
				}

				glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, _depthAttachment, 0);
			}
#endif

		Debug::Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
		               "Framebuffer is incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	});
}

void OpenGLFramebuffer::Bind() const
{
	Shared<const OpenGLFramebuffer> instance = this;
	Renderer::Submit([instance]()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, instance->_rendererID);
		glViewport(0, 0, instance->_width, instance->_height);
	});
}

void OpenGLFramebuffer::Unbind() const
{
	Renderer::Submit([]()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	});
}

void OpenGLFramebuffer::Clear()
{
	Shared<const OpenGLFramebuffer> instance = this;
	Renderer::Submit([instance]()
	{
		const auto& col = instance->_specification.ClearColor;
		glBindFramebuffer(GL_FRAMEBUFFER, instance->_rendererID);
		RendererApi::Clear(col.r, col.g, col.b, col.a);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	});
}

void OpenGLFramebuffer::BindTexture(uint attachmentIndex, uint slot) const
{
	Shared<const OpenGLFramebuffer> instance = this;
	Renderer::Submit([instance, attachmentIndex, slot]()
	{
		glBindTextureUnit(slot, instance->_colorAttachments[attachmentIndex]);
	});
}

uint OpenGLFramebuffer::GetWidth() const
{
	return _specification.Width;
}

uint OpenGLFramebuffer::GetHeight() const
{
	return _specification.Height;
}

RendererID OpenGLFramebuffer::GetRendererID() const
{
	return _rendererID;
}

RendererID OpenGLFramebuffer::GetColorAttachmentRendererID(int index) const
{
	return _colorAttachments[index];
}

RendererID OpenGLFramebuffer::GetDepthAttachmentRendererID() const
{
	return _depthAttachment;
}

const FramebufferSpecification& OpenGLFramebuffer::GetSpecification() const
{
	return _specification;
}
}
