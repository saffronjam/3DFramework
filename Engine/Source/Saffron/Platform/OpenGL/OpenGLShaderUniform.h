#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/Resources/ShaderUniform.h"

namespace Se
{
class OpenGLShaderResourceDeclaration : public ShaderResourceDeclaration
{
public:
	enum class Type
	{
		NONE,
		TEXTURE2D,
		TEXTURECUBE
	};

private:
	friend class OpenGLShader;

public:
	OpenGLShaderResourceDeclaration(Type type, const String& name, uint count);

	const String& GetName() const override;
	uint GetRegister() const override;
	uint GetCount() const override;
	Type GetType() const;

	static Type StringToType(const String& type);
	static String TypeToString(Type type);

private:
	String _name;
	uint _register = 0;
	uint _count;
	Type _type;
};

class OpenGLShaderUniformDeclaration : public ShaderUniformDeclaration
{
private:
	friend class OpenGLShader;
	friend class OpenGLShaderUniformBufferDeclaration;
public:
	enum class Type
	{
		None,
		Float32,
		Vec2,
		Vec3,
		Vec4,
		Mat3,
		Mat4,
		Int32,
		Bool,
		Struct
	};

private:
	String _name;
	uint _size;
	uint _count;
	uint _offset;
	ShaderDomain _domain;

	Type _type;
	ShaderStruct* _struct;
	mutable int32_t _location;
public:
	OpenGLShaderUniformDeclaration(ShaderDomain domain, Type type, const String& name, uint count = 1);
	OpenGLShaderUniformDeclaration(ShaderDomain domain, ShaderStruct* uniformStruct, const String& name,
	                               uint count = 1);

	const String& GetName() const override;
	uint GetSize() const override;
	uint GetCount() const override;

	uint GetOffset() const override;
	uint GetAbsoluteOffset() const;

	ShaderDomain GetDomain() const override;
	int32_t GetLocation() const;
	Type GetType() const;

	bool IsArray() const;

	const ShaderStruct& GetShaderUniformStruct() const;

protected:
	void SetOffset(uint offset) override;

public:
	static uint SizeOfUniformType(Type type);
	static Type StringToType(const String& type);
	static String TypeToString(Type type);
};

struct GLShaderUniformField
{
	OpenGLShaderUniformDeclaration::Type type;
	String name;
	uint count;
	mutable uint size;
	mutable int32_t location;
};

class OpenGLShaderUniformBufferDeclaration : public ShaderUniformBufferDeclaration
{
private:
	friend class Shader;
private:
	String _name;
	ShaderUniformList _uniforms;
	uint _register;
	uint _size;
	ShaderDomain _domain;
public:
	OpenGLShaderUniformBufferDeclaration(const String& name, ShaderDomain domain);

	void PushUniform(OpenGLShaderUniformDeclaration* uniform);

	const String& GetName() const override;
	uint GetRegister() const override;
	uint GetSize() const override;
	virtual ShaderDomain GetDomain() const;
	const ShaderUniformList& GetUniformDeclarations() const override;

	ShaderUniformDeclaration* FindUniform(const String& name) override;
};
}
