#include "Saffron/SaffronPCH.h"

#include <glad/glad.h>

#include <utility>

#include "Saffron/Platform/OpenGL/OpenGLPipeline.h"
#include "Saffron/Renderer/Renderer.h"

namespace Se
{
static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
{
	switch ( type )
	{
	case ShaderDataType::Float:
	case ShaderDataType::Float2:
	case ShaderDataType::Float3:
	case ShaderDataType::Float4:
	case ShaderDataType::Mat3:
	case ShaderDataType::Mat4:		return GL_FLOAT;
	case ShaderDataType::Int:
	case ShaderDataType::Int2:
	case ShaderDataType::Int3:
	case ShaderDataType::Int4:		return GL_INT;
	case ShaderDataType::Bool:		return GL_BOOL;
	default:
		SE_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

}

OpenGLPipeline::OpenGLPipeline(Specification spec)
	: m_Specification(std::move(spec))
{
	OpenGLPipeline::Invalidate();
}

OpenGLPipeline::~OpenGLPipeline()
{
	GLuint rendererID = m_VertexArrayRendererID;
	Renderer::Submit([rendererID]() { glDeleteVertexArrays(1, &rendererID); });
}

void OpenGLPipeline::Bind()
{
	Ref<OpenGLPipeline> instance = this;
	Renderer::Submit([instance]()
					 {
						 glBindVertexArray(instance->m_VertexArrayRendererID);

						 const auto &layout = instance->m_Specification.Layout;
						 Uint32 attribIndex = 0;
						 for ( const auto &element : layout )
						 {
							 const auto glBaseType = ShaderDataTypeToOpenGLBaseType(element.Type);
							 glEnableVertexAttribArray(attribIndex);
							 if ( glBaseType == GL_INT )
							 {
								 glVertexAttribIPointer(attribIndex,
														element.GetComponentCount(),
														glBaseType,
														layout.GetStride(),
														reinterpret_cast<const void *>(static_cast<intptr_t>(element.Offset)));
							 }
							 else
							 {
								 glVertexAttribPointer(attribIndex,
													   element.GetComponentCount(),
													   glBaseType,
													   element.Normalized ? GL_TRUE : GL_FALSE,
													   layout.GetStride(),
													   reinterpret_cast<const void *>(static_cast<intptr_t>(element.Offset)));
							 }
							 attribIndex++;
						 }
					 });
}

void OpenGLPipeline::Invalidate()
{
	SE_CORE_ASSERT(m_Specification.Layout.GetElements().size(), "Layout is empty!");

	Ref<OpenGLPipeline> instance = this;
	Renderer::Submit([instance]() mutable
					 {
						 auto &vertexArrayRendererID = instance->m_VertexArrayRendererID;

						 if ( vertexArrayRendererID )
							 glDeleteVertexArrays(1, &vertexArrayRendererID);

						 glGenVertexArrays(1, &vertexArrayRendererID);
						 glBindVertexArray(vertexArrayRendererID);

						 // TODO: Check whatever this is doing...
#if 0
						 const auto &layout = instance->m_Specification.Layout;
						 uint32_t attribIndex = 0;
						 for ( const auto &element : layout )
						 {
							 const auto glBaseType = ShaderDataTypeToOpenGLBaseType(element.Type);
							 glEnableVertexAttribArray(attribIndex);
							 if ( glBaseType == GL_INT )
							 {
								 glVertexAttribIPointer(attribIndex,
														element.GetComponentCount(),
														glBaseType,
														layout.GetStride(),
														reinterpret_cast<const void *>(static_cast<intptr_t>(element.Offset)));
							 }
							 else
							 {
								 glVertexAttribPointer(attribIndex,
													   element.GetComponentCount(),
													   glBaseType,
													   element.Normalized ? GL_TRUE : GL_FALSE,
													   layout.GetStride(),
													   reinterpret_cast<const void *>(static_cast<intptr_t>(element.Offset)));
							 }
							 attribIndex++;
						 }
#endif
						 glBindVertexArray(0);
					 });
}
}
