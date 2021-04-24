#include "SaffronPCH.h"

#include "Saffron/Platform/OpenGL/OpenGLShaderUniform.h"

namespace Se
{
OpenGLShaderUniformDeclaration::OpenGLShaderUniformDeclaration(ShaderDomain domain, Type type, const String& name,
                                                               Uint32 count) :
	_domain(domain),
	_type(type),
	_struct(nullptr)
{
	_name = name;
	_count = count;
	_size = SizeOfUniformType(type) * count;
}

OpenGLShaderUniformDeclaration::OpenGLShaderUniformDeclaration(ShaderDomain domain, ShaderStruct* uniformStruct,
                                                               const String& name, Uint32 count) :
	_domain(domain),
	_type(Type::Struct),
	_struct(uniformStruct)
{
	_name = name;
	_count = count;
	_size = _struct->GetSize() * count;
}

const String& OpenGLShaderUniformDeclaration::GetName() const
{
	return _name;
}

Uint32 OpenGLShaderUniformDeclaration::GetSize() const
{
	return _size;
}

Uint32 OpenGLShaderUniformDeclaration::GetCount() const
{
	return _count;
}

Uint32 OpenGLShaderUniformDeclaration::GetOffset() const
{
	return _offset;
}

Uint32 OpenGLShaderUniformDeclaration::GetAbsoluteOffset() const
{
	return _struct ? _struct->GetOffset() + _offset : _offset;
}

ShaderDomain OpenGLShaderUniformDeclaration::GetDomain() const
{
	return _domain;
}

int32_t OpenGLShaderUniformDeclaration::GetLocation() const
{
	return _location;
}

OpenGLShaderUniformDeclaration::Type OpenGLShaderUniformDeclaration::GetType() const
{
	return _type;
}

bool OpenGLShaderUniformDeclaration::IsArray() const
{
	return _count > 1;
}

const ShaderStruct& OpenGLShaderUniformDeclaration::GetShaderUniformStruct() const
{
	Debug::Assert(_struct, "");;
	return *_struct;
}

void OpenGLShaderUniformDeclaration::SetOffset(Uint32 offset)
{
	if (_type == Type::Struct) _struct->SetOffset(offset);

	_offset = offset;
}

Uint32 OpenGLShaderUniformDeclaration::SizeOfUniformType(Type type)
{
	switch (type)
	{
	case Type::Bool: return 1;
	case Type::Int32: return 4;
	case Type::Float32: return 4;
	case Type::Vec2: return 4 * 2;
	case Type::Vec3: return 4 * 3;
	case Type::Vec4: return 4 * 4;
	case Type::Mat3: return 4 * 3 * 3;
	case Type::Mat4: return 4 * 4 * 4;
	}
	return 0;
}

OpenGLShaderUniformDeclaration::Type OpenGLShaderUniformDeclaration::StringToType(const String& type)
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

String OpenGLShaderUniformDeclaration::TypeToString(Type type)
{
	switch (type)
	{
	case Type::Int32: return "int32";
	case Type::Bool: return "bool";
	case Type::Float32: return "float";
	case Type::Vec2: return "vec2";
	case Type::Vec3: return "vec3";
	case Type::Vec4: return "vec4";
	case Type::Mat3: return "mat3";
	case Type::Mat4: return "mat4";
	}
	return "Invalid Type";
}

OpenGLShaderUniformBufferDeclaration::OpenGLShaderUniformBufferDeclaration(
	const String& name, ShaderDomain domain) :
	_name(name),
	_register(0),
	_size(0),
	_domain(domain)
{
}

void OpenGLShaderUniformBufferDeclaration::PushUniform(OpenGLShaderUniformDeclaration* uniform)
{
	Uint32 offset = 0;
	if (_uniforms.size())
	{
		OpenGLShaderUniformDeclaration* previous = static_cast<OpenGLShaderUniformDeclaration*>(_uniforms.back());
		offset = previous->_offset + previous->_size;
	}
	uniform->SetOffset(offset);
	_size += uniform->GetSize();
	_uniforms.push_back(uniform);
}

const String& OpenGLShaderUniformBufferDeclaration::GetName() const
{
	return _name;
}

Uint32 OpenGLShaderUniformBufferDeclaration::GetRegister() const
{
	return _register;
}

Uint32 OpenGLShaderUniformBufferDeclaration::GetSize() const
{
	return _size;
}

ShaderDomain OpenGLShaderUniformBufferDeclaration::GetDomain() const
{
	return _domain;
}

const ShaderUniformList& OpenGLShaderUniformBufferDeclaration::GetUniformDeclarations() const
{
	return _uniforms;
}

ShaderUniformDeclaration* OpenGLShaderUniformBufferDeclaration::FindUniform(const String& name)
{
	for (ShaderUniformDeclaration* uniform : _uniforms)
	{
		if (uniform->GetName() == name) return uniform;
	}
	return nullptr;
}

OpenGLShaderResourceDeclaration::OpenGLShaderResourceDeclaration(Type type, const String& name, Uint32 count) :
	_name(name),
	_count(count),
	_type(type)
{
	_name = name;
	_count = count;
}

const String& OpenGLShaderResourceDeclaration::GetName() const
{
	return _name;
}

Uint32 OpenGLShaderResourceDeclaration::GetRegister() const
{
	return _register;
}

Uint32 OpenGLShaderResourceDeclaration::GetCount() const
{
	return _count;
}

OpenGLShaderResourceDeclaration::Type OpenGLShaderResourceDeclaration::GetType() const
{
	return _type;
}

OpenGLShaderResourceDeclaration::Type OpenGLShaderResourceDeclaration::StringToType(const String& type)
{
	if (type == "sampler2D") return Type::TEXTURE2D;
	if (type == "sampler2DMS") return Type::TEXTURE2D;
	if (type == "samplerCube") return Type::TEXTURECUBE;

	return Type::NONE;
}

String OpenGLShaderResourceDeclaration::TypeToString(Type type)
{
	switch (type)
	{
	case Type::TEXTURE2D: return "sampler2D";
	case Type::TEXTURECUBE: return "samplerCube";
	}
	return "Invalid Type";
}
}
