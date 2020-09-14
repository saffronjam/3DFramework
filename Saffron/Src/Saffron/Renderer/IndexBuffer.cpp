#include "Saffron/SaffronPCH.h"

#include "Saffron/Platform/OpenGL/OpenGLIndexBuffer.h"
#include "Saffron/Renderer/IndexBuffer.h"

namespace Se
{

Ref<IndexBuffer> IndexBuffer::Create(Uint32 size)
{
	switch ( RendererAPI::CurrentAPI() )
	{
	case RendererAPI::Type::None:	SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::Type::OpenGL:	return Ref<OpenGLIndexBuffer>::Create(size);
	default:						SE_CORE_ASSERT(false, "Unknown RendererAPI"); return nullptr;
	}
}

Ref<IndexBuffer> IndexBuffer::Create(void *data, Uint32 size)
{
	switch ( RendererAPI::CurrentAPI() )
	{
	case RendererAPI::Type::None:	SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::Type::OpenGL:	return Ref<OpenGLIndexBuffer>::Create(data, size);
	default:						SE_CORE_ASSERT(false, "Unknown RendererAPI!"); return nullptr;
	}

}

Ref<IndexBuffer> IndexBuffer::Create(const Buffer &buffer)
{
	switch ( RendererAPI::CurrentAPI() )
	{
	case RendererAPI::Type::None:	SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	case RendererAPI::Type::OpenGL:	return Ref<OpenGLIndexBuffer>::Create(buffer);
	default:						SE_CORE_ASSERT(false, "Unknown RendererAPI!"); return nullptr;
	}
}
}
