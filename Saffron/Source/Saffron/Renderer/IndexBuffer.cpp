#include "SaffronPCH.h"

#include "Saffron/Platform/OpenGL/OpenGLIndexBuffer.h"
#include "Saffron/Renderer/IndexBuffer.h"

namespace Se
{

std::shared_ptr<IndexBuffer> IndexBuffer::Create(Uint32 size)
{
	switch ( RendererAPI::Current() )
	{
	case RendererAPI::Type::None:	SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::Type::OpenGL:	return CreateShared<OpenGLIndexBuffer>(size);
	default:						SE_CORE_ASSERT(false, "Unknown RendererAPI"); return nullptr;
	}
}

std::shared_ptr<IndexBuffer> IndexBuffer::Create(void *data, Uint32 size)
{
	switch ( RendererAPI::Current() )
	{
	case RendererAPI::Type::None:	SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::Type::OpenGL:	return CreateShared<OpenGLIndexBuffer>(data, size);
	default:						SE_CORE_ASSERT(false, "Unknown RendererAPI!"); return nullptr;
	}

}

std::shared_ptr<IndexBuffer> IndexBuffer::Create(const Buffer &buffer)
{
	switch ( RendererAPI::Current() )
	{
	case RendererAPI::Type::None:	SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::Type::OpenGL:	return CreateShared<OpenGLIndexBuffer>(buffer);
	default:						SE_CORE_ASSERT(false, "Unknown RendererAPI!"); return nullptr;
	}
}
}
