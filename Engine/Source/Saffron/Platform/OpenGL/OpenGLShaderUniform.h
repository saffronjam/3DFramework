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
	OpenGLShaderResourceDeclaration(Type type, const String& name, Uint32 count);

	const String& GetName() const override;
	Uint32 GetRegister() const override;
	Uint32 GetCount() const override;
	Type GetType() const;

	static Type StringToType(const String& type);
	static String TypeToString(Type type);

private:
	String m_Name;
	Uint32 m_Register = 0;
	Uint32 m_Count;
	Type m_Type;
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
	String m_Name;
	Uint32 m_Size;
	Uint32 m_Count;
	Uint32 m_Offset;
	ShaderDomain m_Domain;

	Type m_Type;
	ShaderStruct* m_Struct;
	mutable int32_t m_Location;
public:
	OpenGLShaderUniformDeclaration(ShaderDomain domain, Type type, const String& name, Uint32 count = 1);
	OpenGLShaderUniformDeclaration(ShaderDomain domain, ShaderStruct* uniformStruct, const String& name,
	                               Uint32 count = 1);

	const String& GetName() const override;
	Uint32 GetSize() const override;
	Uint32 GetCount() const override;

	Uint32 GetOffset() const override;
	Uint32 GetAbsoluteOffset() const;

	ShaderDomain GetDomain() const override;
	int32_t GetLocation() const;
	Type GetType() const;

	bool IsArray() const;

	const ShaderStruct& GetShaderUniformStruct() const;

protected:
	void SetOffset(Uint32 offset) override;

public:
	static Uint32 SizeOfUniformType(Type type);
	static Type StringToType(const String& type);
	static String TypeToString(Type type);
};

struct GLShaderUniformField
{
	OpenGLShaderUniformDeclaration::Type type;
	String name;
	Uint32 count;
	mutable Uint32 size;
	mutable int32_t location;
};

class OpenGLShaderUniformBufferDeclaration : public ShaderUniformBufferDeclaration
{
private:
	friend class Shader;
private:
	String m_Name;
	ShaderUniformList m_Uniforms;
	Uint32 m_Register;
	Uint32 m_Size;
	ShaderDomain m_Domain;
public:
	OpenGLShaderUniformBufferDeclaration(const String& name, ShaderDomain domain);

	void PushUniform(OpenGLShaderUniformDeclaration* uniform);

	const String& GetName() const override;
	Uint32 GetRegister() const override;
	Uint32 GetSize() const override;
	virtual ShaderDomain GetDomain() const;
	const ShaderUniformList& GetUniformDeclarations() const override;

	ShaderUniformDeclaration* FindUniform(const String& name) override;
};
}
