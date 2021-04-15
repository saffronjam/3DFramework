#include "SaffronPCH.h"

#include "Saffron/Platform/OpenGL/OpenGLShaderUniform.h"

namespace Se
{
//////////////////////////////////////////
/// OpenGL Shader Resource Declaration ///
//////////////////////////////////////////
OpenGLShaderResourceDeclaration::OpenGLShaderResourceDeclaration(Type type, String name, Uint32 count)
	: m_Name(Move(name)), m_Count(count), m_Type(type)
{
}

OpenGLShaderResourceDeclaration::Type OpenGLShaderResourceDeclaration::StringToType(const String &type)
{
	if ( type == "sampler2D" )    return Type::Texture2D;
	if ( type == "sampler2DMS" )  return Type::Texture2D;
	if ( type == "samplerCube" )  return Type::TextureCube;

	return Type::None;
}

String OpenGLShaderResourceDeclaration::TypeToString(Type type)
{
	switch ( type )
	{
	case Type::Texture2D:		return "sampler2D";
	case Type::TextureCube:		return "samplerCube";
	default:					return "Invalid Type";
	}
}

/////////////////////////////////////////
/// OpenGL Shader Uniform Declaration ///
/////////////////////////////////////////
OpenGLShaderUniformDeclaration::OpenGLShaderUniformDeclaration(ShaderDomain domain, Type type,
															   String name, Uint32 count)
	:
	m_Struct(nullptr),
	m_Name(Move(name)),
	m_Count(count),
	m_Size(SizeOfUniformType(type) *count),
	m_Domain(domain),
	m_Type(type)

{
}

OpenGLShaderUniformDeclaration::OpenGLShaderUniformDeclaration(ShaderDomain domain, ShaderStruct *uniformStruct,
															   String name, Uint32 count)
	:
	m_Struct(uniformStruct),
	m_Name(Move(name)),
	m_Count(count),
	m_Size(m_Struct->GetSize() *count),
	m_Domain(domain),
	m_Type(Type::Struct)
{
}

void OpenGLShaderUniformDeclaration::SetOffset(Uint32 offset)
{
	if ( m_Type == Type::Struct )
		m_Struct->SetOffset(offset);
	m_Offset = offset;
}

Uint32 OpenGLShaderUniformDeclaration::SizeOfUniformType(Type type)
{
	switch ( type )
	{
	case Type::Bool:		return 1;
	case Type::Int32:		return 4;
	case Type::Float32:		return 4;
	case Type::Vec2:		return 4 * 2;
	case Type::Vec3:		return 4 * 3;
	case Type::Vec4:		return 4 * 4;
	case Type::Mat3:		return 4 * 3 * 3;
	case Type::Mat4:		return 4 * 4 * 4;
	default:				return 0;
	}
}

OpenGLShaderUniformDeclaration::Type OpenGLShaderUniformDeclaration::StringToType(const String &type)
{
	if ( type == "bool" )      return Type::Bool;
	if ( type == "int" )      return Type::Int32;
	if ( type == "float" )    return Type::Float32;
	if ( type == "vec2" )     return Type::Vec2;
	if ( type == "vec3" )     return Type::Vec3;
	if ( type == "vec4" )     return Type::Vec4;
	if ( type == "mat3" )     return Type::Mat3;
	if ( type == "mat4" )     return Type::Mat4;

	return Type::None;
}

String OpenGLShaderUniformDeclaration::TypeToString(Type type)
{
	switch ( type )
	{
	case Type::Bool:		return "bool";
	case Type::Int32:		return "int32";
	case Type::Float32:		return "float";
	case Type::Vec2:		return "vec2";
	case Type::Vec3:		return "vec3";
	case Type::Vec4:		return "vec4";
	case Type::Mat3:		return "mat3";
	case Type::Mat4:		return "mat4";
	default:				return "Invalid Type";
	}

}

////////////////////////////////////////////////
/// OpenGL Shader Uniform Buffer Declaration ///
////////////////////////////////////////////////
OpenGLShaderUniformBufferDeclaration::OpenGLShaderUniformBufferDeclaration(String name,
																		   ShaderDomain domain)
	: m_Name(Move(name)), m_Register(0), m_Size(0), m_Domain(domain)

{
}

void OpenGLShaderUniformBufferDeclaration::PushUniform(OpenGLShaderUniformDeclaration *uniform)
{
	Uint32 offset = 0;
	if ( !m_Uniforms.empty() )
	{
		auto *previous = dynamic_cast<OpenGLShaderUniformDeclaration *>(m_Uniforms.back());
		offset = previous->m_Offset + previous->m_Size;
	}
	uniform->SetOffset(offset);
	m_Size += uniform->GetSize();
	m_Uniforms.push_back(uniform);
}

ShaderUniformDeclaration *OpenGLShaderUniformBufferDeclaration::FindUniform(const String &name)
{
	for ( auto *uniform : m_Uniforms )
	{
		if ( uniform->GetName() == name )
			return uniform;
	}
	return nullptr;
}
}
