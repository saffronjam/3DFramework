#include "SaffronPCH.h"

#include "Saffron/Platform/OpenGL/OpenGLIndexBuffer.h"
#include "Saffron/Rendering/Resources/IndexBuffer.h"

namespace Se
{
Shared<IndexBuffer> IndexBuffer::Create(Uint32 size)
{
	switch (RendererApi::Current())
	{
	case RendererApiType::None: return nullptr;
	case RendererApiType::OpenGL: return Shared<OpenGLIndexBuffer>::Create(size);
	}
	SE_CORE_FALSE_ASSERT("Unknown RendererAPI");
	return nullptr;
}

Shared<IndexBuffer> IndexBuffer::Create(void* data, Uint32 size)
{
	switch (RendererApi::Current())
	{
	case RendererApiType::None: return nullptr;
	case RendererApiType::OpenGL: return Shared<OpenGLIndexBuffer>::Create(data, size);
	}
	SE_CORE_FALSE_ASSERT("Unknown RendererAPI");
	return nullptr;
}

Shared<IndexBuffer> IndexBuffer::Create(const Buffer& buffer)
{
	switch (RendererApi::Current())
	{
	case RendererApiType::None: return nullptr;
	case RendererApiType::OpenGL: return Shared<OpenGLIndexBuffer>::Create(buffer);
	}
	SE_CORE_FALSE_ASSERT("Unknown RendererAPI");
	return nullptr;
}
}
