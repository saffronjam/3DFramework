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
	Debug::Break("Unknown RendererAPI");
	return nullptr;
}

Shared<IndexBuffer> IndexBuffer::Create(void* data, Uint32 size)
{
	switch (RendererApi::Current())
	{
	case RendererApiType::None: return nullptr;
	case RendererApiType::OpenGL: return Shared<OpenGLIndexBuffer>::Create(data, size);
	}
	Debug::Break("Unknown RendererAPI");
	return nullptr;
}

Shared<IndexBuffer> IndexBuffer::Create(const Buffer& buffer)
{
	switch (RendererApi::Current())
	{
	case RendererApiType::None: return nullptr;
	case RendererApiType::OpenGL: return Shared<OpenGLIndexBuffer>::Create(buffer);
	}
	Debug::Break("Unknown RendererAPI");
	return nullptr;
}
}
