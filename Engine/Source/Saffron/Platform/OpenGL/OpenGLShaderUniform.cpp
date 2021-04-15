#include "SaffronPCH.h"

#include "Saffron/Platform/OpenGL/OpenGLShaderUniform.h"

namespace Se
{
OpenGLShaderUniformDeclaration::OpenGLShaderUniformDeclaration(ShaderDomain domain, Type type, const std::string& name,
                                                               uint32_t count) :
	m_Type(type),
	m_Struct(nullptr),
	m_Domain(domain)
{
	m_Name = name;
	m_Count = count;
	m_Size = SizeOfUniformType(type) * count;
}

OpenGLShaderUniformDeclaration::OpenGLShaderUniformDeclaration(ShaderDomain domain, ShaderStruct* uniformStruct,
                                                               const std::string& name, uint32_t count) :
	m_Struct(uniformStruct),
	m_Type(OpenGLShaderUniformDeclaration::Type::Struct),
	m_Domain(domain)
{
	m_Name = name;
	m_Count = count;
	m_Size = m_Struct->GetSize() * count;
}

const std::string& OpenGLShaderUniformDeclaration::GetName() const
{
	return m_Name;
}

uint32_t OpenGLShaderUniformDeclaration::GetSize() const
{
	return m_Size;
}

uint32_t OpenGLShaderUniformDeclaration::GetCount() const
{
	return m_Count;
}

uint32_t OpenGLShaderUniformDeclaration::GetOffset() const
{
	return m_Offset;
}

uint32_t OpenGLShaderUniformDeclaration::GetAbsoluteOffset() const
{
	return m_Struct ? m_Struct->GetOffset() + m_Offset : m_Offset;
}

ShaderDomain OpenGLShaderUniformDeclaration::GetDomain() const
{
	return m_Domain;
}

int32_t OpenGLShaderUniformDeclaration::GetLocation() const
{
	return m_Location;
}

OpenGLShaderUniformDeclaration::Type OpenGLShaderUniformDeclaration::GetType() const
{
	return m_Type;
}

bool OpenGLShaderUniformDeclaration::IsArray() const
{
	return m_Count > 1;
}

const ShaderStruct& OpenGLShaderUniformDeclaration::GetShaderUniformStruct() const
{
	SE_CORE_ASSERT(m_Struct, "");
	return *m_Struct;
}

void OpenGLShaderUniformDeclaration::SetOffset(uint32_t offset)
{
	if (m_Type == OpenGLShaderUniformDeclaration::Type::Struct) m_Struct->SetOffset(offset);

	m_Offset = offset;
}

uint32_t OpenGLShaderUniformDeclaration::SizeOfUniformType(Type type)
{
	switch (type)
	{
	case OpenGLShaderUniformDeclaration::Type::Bool: return 1;
	case OpenGLShaderUniformDeclaration::Type::Int32: return 4;
	case OpenGLShaderUniformDeclaration::Type::Float32: return 4;
	case OpenGLShaderUniformDeclaration::Type::Vec2: return 4 * 2;
	case OpenGLShaderUniformDeclaration::Type::Vec3: return 4 * 3;
	case OpenGLShaderUniformDeclaration::Type::Vec4: return 4 * 4;
	case OpenGLShaderUniformDeclaration::Type::Mat3: return 4 * 3 * 3;
	case OpenGLShaderUniformDeclaration::Type::Mat4: return 4 * 4 * 4;
	}
	return 0;
}

OpenGLShaderUniformDeclaration::Type OpenGLShaderUniformDeclaration::StringToType(const std::string& type)
{
	if (type == "int") return Type::Int32;
	if (type == "bool") return Type::Bool;
	if (type == "float") return Type::Float32;
	if (type == "vec2") return Type::Vec2;
	if (type == "vec3") return Type::Vec3;
	if (type == "vec4") return Type::Vec4;
	if (type == "mat3") return Type::Mat3;
	if (type == "mat4") return Type::Mat4;

	return Type::None;
}

std::string OpenGLShaderUniformDeclaration::TypeToString(Type type)
{
	switch (type)
	{
	case OpenGLShaderUniformDeclaration::Type::Int32: return "int32";
	case OpenGLShaderUniformDeclaration::Type::Bool: return "bool";
	case OpenGLShaderUniformDeclaration::Type::Float32: return "float";
	case OpenGLShaderUniformDeclaration::Type::Vec2: return "vec2";
	case OpenGLShaderUniformDeclaration::Type::Vec3: return "vec3";
	case OpenGLShaderUniformDeclaration::Type::Vec4: return "vec4";
	case OpenGLShaderUniformDeclaration::Type::Mat3: return "mat3";
	case OpenGLShaderUniformDeclaration::Type::Mat4: return "mat4";
	}
	return "Invalid Type";
}

OpenGLShaderUniformBufferDeclaration::OpenGLShaderUniformBufferDeclaration(
	const std::string& name, ShaderDomain domain) :
	m_Name(name),
	m_Domain(domain),
	m_Size(0),
	m_Register(0)
{
}

void OpenGLShaderUniformBufferDeclaration::PushUniform(OpenGLShaderUniformDeclaration* uniform)
{
	uint32_t offset = 0;
	if (m_Uniforms.size())
	{
		OpenGLShaderUniformDeclaration* previous = (OpenGLShaderUniformDeclaration*)m_Uniforms.back();
		offset = previous->m_Offset + previous->m_Size;
	}
	uniform->SetOffset(offset);
	m_Size += uniform->GetSize();
	m_Uniforms.push_back(uniform);
}

ShaderUniformDeclaration* OpenGLShaderUniformBufferDeclaration::FindUniform(const std::string& name)
{
	for (ShaderUniformDeclaration* uniform : m_Uniforms)
	{
		if (uniform->GetName() == name) return uniform;
	}
	return nullptr;
}

OpenGLShaderResourceDeclaration::OpenGLShaderResourceDeclaration(Type type, const std::string& name, uint32_t count) :
	m_Type(type),
	m_Name(name),
	m_Count(count)
{
	m_Name = name;
	m_Count = count;
}

const std::string& OpenGLShaderResourceDeclaration::GetName() const
{
	return m_Name;
}

uint32_t OpenGLShaderResourceDeclaration::GetRegister() const
{
	return m_Register;
}

uint32_t OpenGLShaderResourceDeclaration::GetCount() const
{
	return m_Count;
}

OpenGLShaderResourceDeclaration::Type OpenGLShaderResourceDeclaration::GetType() const
{
	return m_Type;
}

OpenGLShaderResourceDeclaration::Type OpenGLShaderResourceDeclaration::StringToType(const std::string& type)
{
	if (type == "sampler2D") return Type::TEXTURE2D;
	if (type == "sampler2DMS") return Type::TEXTURE2D;
	if (type == "samplerCube") return Type::TEXTURECUBE;

	return Type::NONE;
}

std::string OpenGLShaderResourceDeclaration::TypeToString(Type type)
{
	switch (type)
	{
	case Type::TEXTURE2D: return "sampler2D";
	case Type::TEXTURECUBE: return "samplerCube";
	}
	return "Invalid Type";
}
}
