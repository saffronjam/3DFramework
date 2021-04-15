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
	OpenGLShaderResourceDeclaration(Type type, const std::string& name, uint32_t count);

	const std::string& GetName() const override;
	uint32_t GetRegister() const override;
	uint32_t GetCount() const override;
	Type GetType() const;

	static Type StringToType(const std::string& type);
	static std::string TypeToString(Type type);

private:
	std::string m_Name;
	uint32_t m_Register = 0;
	uint32_t m_Count;
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
	std::string m_Name;
	uint32_t m_Size;
	uint32_t m_Count;
	uint32_t m_Offset;
	ShaderDomain m_Domain;

	Type m_Type;
	ShaderStruct* m_Struct;
	mutable int32_t m_Location;
public:
	OpenGLShaderUniformDeclaration(ShaderDomain domain, Type type, const std::string& name, uint32_t count = 1);
	OpenGLShaderUniformDeclaration(ShaderDomain domain, ShaderStruct* uniformStruct, const std::string& name,
	                               uint32_t count = 1);

	const std::string& GetName() const override;
	uint32_t GetSize() const override;
	uint32_t GetCount() const override;

	uint32_t GetOffset() const override;
	uint32_t GetAbsoluteOffset() const;

	ShaderDomain GetDomain() const override;
	int32_t GetLocation() const;
	Type GetType() const;

	bool IsArray() const;

	const ShaderStruct& GetShaderUniformStruct() const;

protected:
	void SetOffset(uint32_t offset) override;

public:
	static uint32_t SizeOfUniformType(Type type);
	static Type StringToType(const std::string& type);
	static std::string TypeToString(Type type);
};

struct GLShaderUniformField
{
	OpenGLShaderUniformDeclaration::Type type;
	std::string name;
	uint32_t count;
	mutable uint32_t size;
	mutable int32_t location;
};

class OpenGLShaderUniformBufferDeclaration : public ShaderUniformBufferDeclaration
{
private:
	friend class Shader;
private:
	std::string m_Name;
	ShaderUniformList m_Uniforms;
	uint32_t m_Register;
	uint32_t m_Size;
	ShaderDomain m_Domain;
public:
	OpenGLShaderUniformBufferDeclaration(const std::string& name, ShaderDomain domain);

	void PushUniform(OpenGLShaderUniformDeclaration* uniform);

	const std::string& GetName() const override { return m_Name; }

	uint32_t GetRegister() const override { return m_Register; }

	uint32_t GetSize() const override { return m_Size; }

	virtual ShaderDomain GetDomain() const { return m_Domain; }

	const ShaderUniformList& GetUniformDeclarations() const override { return m_Uniforms; }

	ShaderUniformDeclaration* FindUniform(const std::string& name) override;
};
}
