#pragma once

#include "Saffron/Base.h"
#include "Saffron/Renderer/ShaderUniform.h"

namespace Se
{
//////////////////////////////////////////
/// OpenGL Shader Resource Declaration ///
//////////////////////////////////////////
class OpenGLShaderResourceDeclaration : public ShaderResourceDeclaration
{
public:
	enum class Type
	{
		None, Texture2D, TextureCube
	};

private:
	friend class OpenGLShader;

private:
	std::string m_Name;
	Uint32 m_Register = 0;
	Uint32 m_Count;
	Type m_Type;

public:
	OpenGLShaderResourceDeclaration(Type type, std::string name, Uint32 count);

	const std::string &GetName() const override { return m_Name; }
	Uint32 GetRegister() const override { return m_Register; }
	Uint32 GetCount() const override { return m_Count; }

	Type GetType() const { return m_Type; }
public:
	static Type StringToType(const std::string &type);
	static std::string TypeToString(Type type);
};

/////////////////////////////////////////
/// OpenGL Shader Uniform Declaration ///
/////////////////////////////////////////
class OpenGLShaderUniformDeclaration : public ShaderUniformDeclaration
{
private:
	friend class OpenGLShader;
	friend class OpenGLShaderUniformBufferDeclaration;

public:
	enum class Type
	{
		None, Float32, Vec2, Vec3, Vec4, Mat3, Mat4, Int32, Struct
	};

private:
	ShaderStruct *m_Struct;

	std::string m_Name;
	Uint32 m_Count;
	Uint32 m_Size;
	Uint32 m_Offset{};
	ShaderDomain m_Domain;

	Type m_Type;
	mutable Int32 m_Location{};

public:
	OpenGLShaderUniformDeclaration(ShaderDomain domain, Type type, std::string name, Uint32 count = 1);
	OpenGLShaderUniformDeclaration(ShaderDomain domain, ShaderStruct *uniformStruct, std::string name, Uint32 count = 1);

	const std::string &GetName() const override { return m_Name; }
	Uint32 GetSize() const override { return m_Size; }
	Uint32 GetCount() const override { return m_Count; }
	Uint32 GetOffset() const override { return m_Offset; }
	Uint32 GetAbsoluteOffset() const { return m_Struct ? m_Struct->GetOffset() + m_Offset : m_Offset; }
	ShaderDomain GetDomain() const override { return m_Domain; }

	Int32 GetLocation() const { return m_Location; }
	Type GetType() const { return m_Type; }
	bool IsArray() const { return m_Count > 1; }
	const ShaderStruct &GetShaderUniformStruct() const { SE_CORE_ASSERT(m_Struct, ""); return *m_Struct; }
protected:
	void SetOffset(Uint32 offset) override;
public:
	static Uint32 SizeOfUniformType(Type type);
	static Type StringToType(const std::string &type);
	static std::string TypeToString(Type type);
};

///////////////////////////////////
/// OpenGL Shader Uniform Field ///
///////////////////////////////////
struct GLShaderUniformField
{
	OpenGLShaderUniformDeclaration::Type Type;
	std::string Name;
	Uint32 Count;
	mutable Uint32 Size;
	mutable Int32 Location;
};

////////////////////////////////////////////////
/// OpenGL Shader Uniform Buffer Declaration ///
////////////////////////////////////////////////
class OpenGLShaderUniformBufferDeclaration : public ShaderUniformBufferDeclaration
{
private:
	friend class Shader;
private:
	std::string m_Name;
	ShaderUniformDeclaration::List m_Uniforms;
	Uint32 m_Register;
	Uint32 m_Size;
	ShaderDomain m_Domain;
public:
	OpenGLShaderUniformBufferDeclaration(std::string name, ShaderDomain domain);

	void PushUniform(OpenGLShaderUniformDeclaration *uniform);

	const std::string &GetName() const override { return m_Name; }
	Uint32 GetRegister() const override { return m_Register; }
	Uint32 GetSize() const override { return m_Size; }
	ShaderDomain GetDomain() const { return m_Domain; }
	const ShaderUniformDeclaration::List &GetUniformDeclarations() const override { return m_Uniforms; }

	ShaderUniformDeclaration *FindUniform(const std::string &name) override;
};

}

