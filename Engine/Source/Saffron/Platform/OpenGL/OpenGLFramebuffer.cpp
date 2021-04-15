#include "SaffronPCH.h"

#include <glad/glad.h>

#include "Saffron/Platform/OpenGL/OpenGLFramebuffer.h"
#include "Saffron/Renderer/Renderer.h"


namespace Se
{

namespace Utils
{
static GLenum TextureTarget(bool multisampled)
{
	return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
}

static void CreateTextures(bool multisampled, RendererID *outID, uint32_t count)
{
	glCreateTextures(TextureTarget(multisampled), 1, outID);
}

static void BindTexture(bool multisampled, RendererID id)
{
	glBindTexture(TextureTarget(multisampled), id);
}

static GLenum DataType(GLenum format)
{
	switch ( format )
	{
	case GL_RGBA8: return GL_UNSIGNED_BYTE;
	case GL_RG16F:
	case GL_RG32F:
	case GL_RGBA16F:
	case GL_RGBA32F: return GL_FLOAT;
	case GL_DEPTH24_STENCIL8: return GL_UNSIGNED_INT_24_8;
	default: SE_CORE_ASSERT(false, "Unknown format!"); return 0;
	}
}

static void AttachColorTexture(RendererID id, int samples, GLenum format, uint32_t width, uint32_t height, int index)
{
	const bool multisampled = samples > 1;
	if ( multisampled )
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

static void AttachDepthTexture(RendererID id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
{
	const bool multisampled = samples > 1;
	if ( multisampled )
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

static bool IsDepthFormat(Framebuffer::TextureFormat format)
{
	switch ( format )
	{
	case Framebuffer::TextureFormat::DEPTH24STENCIL8:
	case Framebuffer::TextureFormat::DEPTH32F:
		return true;
	default:
		return false;
	}
}

}

OpenGLFramebuffer::OpenGLFramebuffer(const Specification &spec)
	: m_Specification(spec), m_Width(spec.Width), m_Height(spec.Height)
{
	for ( const auto &format : m_Specification.Attachments.Attachments )
	{
		if ( !Utils::IsDepthFormat(format.TextureFormat) )
			m_ColorAttachmentFormats.emplace_back(format.TextureFormat);
		else
			m_DepthAttachmentFormat = format.TextureFormat;
	}

	m_Initializer = [this]
	{
		OpenGLFramebuffer::Resize(m_Width, m_Height, true);
	};
}

OpenGLFramebuffer::~OpenGLFramebuffer()
{
	auto rendererID = m_RendererID;
	Renderer::Submit([rendererID]() {	glDeleteFramebuffers(1, &rendererID); });
}

void OpenGLFramebuffer::Resize(Uint32 width, Uint32 height, bool forceRecreate)
{
	if ( !forceRecreate && (m_Specification.Width == width && m_Specification.Height == height) )
	{
		return;
	}

	m_Width = width;
	m_Height = height;
	auto instance = GetDynShared<OpenGLFramebuffer>();
	Renderer::Submit([instance]() mutable
					 {
						 if ( instance->m_RendererID )
						 {
							 glDeleteFramebuffers(1, &instance->m_RendererID);
							 glDeleteTextures(instance->m_ColorAttachments.size(), instance->m_ColorAttachments.data());
							 glDeleteTextures(1, &instance->m_DepthAttachment);

							 instance->m_ColorAttachments.clear();
							 instance->m_DepthAttachment = 0;
						 }

						 glGenFramebuffers(1, &instance->m_RendererID);
						 glBindFramebuffer(GL_FRAMEBUFFER, instance->m_RendererID);

						 bool multisample = instance->m_Specification.Samples > 1;

						 if ( instance->m_ColorAttachmentFormats.size() )
						 {
							 instance->m_ColorAttachments.resize(instance->m_ColorAttachmentFormats.size());
							 Utils::CreateTextures(multisample, instance->m_ColorAttachments.data(), instance->m_ColorAttachments.size());

							 // Create color attachments
							 for ( int i = 0; i < instance->m_ColorAttachments.size(); i++ )
							 {
								 Utils::BindTexture(multisample, instance->m_ColorAttachments[i]);
								 switch ( instance->m_ColorAttachmentFormats[i] )
								 {
								 case TextureFormat::RGBA8:
									 Utils::AttachColorTexture(instance->m_ColorAttachments[i], instance->m_Specification.Samples, GL_RGBA8, instance->m_Width, instance->m_Height, i);
									 break;
								 case TextureFormat::RGBA16F:
									 Utils::AttachColorTexture(instance->m_ColorAttachments[i], instance->m_Specification.Samples, GL_RGBA16F, instance->m_Width, instance->m_Height, i);
									 break;
								 case TextureFormat::RGBA32F:
									 Utils::AttachColorTexture(instance->m_ColorAttachments[i], instance->m_Specification.Samples, GL_RGBA32F, instance->m_Width, instance->m_Height, i);
									 break;
								 case TextureFormat::RG32F:
									 Utils::AttachColorTexture(instance->m_ColorAttachments[i], instance->m_Specification.Samples, GL_RG32F, instance->m_Width, instance->m_Height, i);
									 break;
								 }

							 }
						 }

						 if ( instance->m_DepthAttachmentFormat != TextureFormat::None )
						 {
							 Utils::CreateTextures(multisample, &instance->m_DepthAttachment, 1);
							 Utils::BindTexture(multisample, instance->m_DepthAttachment);
							 switch ( instance->m_DepthAttachmentFormat )
							 {
							 case TextureFormat::DEPTH24STENCIL8:
								 Utils::AttachDepthTexture(instance->m_DepthAttachment, instance->m_Specification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, instance->m_Width, instance->m_Height);
								 break;
							 case TextureFormat::DEPTH32F:
								 Utils::AttachDepthTexture(instance->m_DepthAttachment, instance->m_Specification.Samples, GL_DEPTH_COMPONENT32F, GL_DEPTH_ATTACHMENT, instance->m_Width, instance->m_Height);
								 break;
							 }

						 }

						 if ( instance->m_ColorAttachments.size() > 1 )
						 {
							 SE_CORE_ASSERT(instance->m_ColorAttachments.size() <= 4);
							 GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
							 glDrawBuffers(instance->m_ColorAttachments.size(), buffers);
						 }
						 else if ( instance->m_ColorAttachments.size() == 0 )
						 {
							 // Only depth-pass
							 glDrawBuffer(GL_NONE);
						 }

#if 0

						 if ( multisample )
						 {
							 glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_ColorAttachment);
							 glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachment);

							 // TODO: Create texture object based on format here
							 if ( m_Specification.Format == FramebufferFormat::RGBA16F )
							 {
								 glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Specification.Samples, GL_RGBA16F, m_Specification.Width, m_Specification.Height, GL_FALSE);
							 }
							 else if ( m_Specification.Format == FramebufferFormat::RGBA8 )
							 {
								 glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Specification.Samples, GL_RGBA8, m_Specification.Width, m_Specification.Height, GL_FALSE);
							 }
							 // glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
							 // glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
							 glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
						 }
						 else
						 {
							 glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);
							 glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);

							 // TODO: Create texture object based on format here
							 if ( m_Specification.Format == FramebufferFormat::RGBA16F )
							 {
								 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Specification.Width, m_Specification.Height, 0, GL_RGBA, GL_FLOAT, nullptr);
							 }
							 else if ( m_Specification.Format == FramebufferFormat::RG32F ) // "Shadow" for now
							 {
								 glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, m_Specification.Width, m_Specification.Height);
								 glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_ColorAttachment, 0);
								 glDrawBuffer(GL_NONE);
							 }
							 else if ( m_Specification.Format == FramebufferFormat::RGBA8 )
							 {
								 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Specification.Width, m_Specification.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
							 }
							 else if ( m_Specification.Format == FramebufferFormat::COMP )
							 {
								 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Specification.Width, m_Specification.Height, 0, GL_RGBA, GL_FLOAT, nullptr);
								 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
								 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

								 glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment2);
								 glBindTexture(GL_TEXTURE_2D, m_ColorAttachment2);
								 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Specification.Width, m_Specification.Height, 0, GL_RGBA, GL_FLOAT, nullptr);
								 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
								 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

							 }

							 if ( m_Specification.Format != FramebufferFormat::RG32F )
							 {
								 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
								 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
								 glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);
							 }
						 }

						 if ( multisample )
						 {
							 glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_DepthAttachment);
							 glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_DepthAttachment);
							 glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Specification.Samples, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height, GL_FALSE);
							 glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
						 }
						 else if ( m_Specification.Format != FramebufferFormat::RG32F )
						 {
							 glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
							 glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
							 glTexImage2D(
								 GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height, 0,
								 GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
							 );
							 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
							 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
							 //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);
						 }

						 if ( m_Specification.Format != FramebufferFormat::RG32F )
						 {
							 if ( multisample )
							 {
								 glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachment, 0);
							 }
							 else
							 {
								 glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_ColorAttachment, 0);
								 if ( m_Specification.Format == FramebufferFormat::COMP )
								 {
									 glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_ColorAttachment2, 0);
									 const GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
									 glDrawBuffers(2, buffers);
								 }
							 }

							 glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, m_DepthAttachment, 0);
						 }
#endif

						 SE_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

						 glBindFramebuffer(GL_FRAMEBUFFER, 0);
					 });
}

void OpenGLFramebuffer::Bind() const
{
	auto instance = GetDynShared<const OpenGLFramebuffer>();
	Renderer::Submit([instance]()
					 {
						 glBindFramebuffer(GL_FRAMEBUFFER, instance->m_RendererID);
						 glViewport(0, 0, instance->m_Width, instance->m_Height);
					 });
}

void OpenGLFramebuffer::Unbind() const
{
	Renderer::Submit([=]()
					 {
						 glBindFramebuffer(GL_FRAMEBUFFER, 0);
					 });
}

void OpenGLFramebuffer::BindTexture(Uint32 attachmentIndex, Uint32 slot) const
{
	auto instance = GetDynShared<const OpenGLFramebuffer>();
	Renderer::Submit([instance, attachmentIndex, slot]()
					 {
						 glBindTextureUnit(slot, instance->m_ColorAttachments[attachmentIndex]);
					 });
}
}
