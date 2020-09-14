#pragma once

#include <string>
#include <vector>

#include "Saffron/Config.h"

namespace Se
{

enum class ShaderDomain
{
	None = 0,
	Vertex = 0,
	Pixel = 1
};

//////////////////////////////////
/// Shader Uniform Declaration ///
//////////////////////////////////
class ShaderUniformDeclaration
{
public:
	using List = std::vector<ShaderUniformDeclaration *>;

private:
	friend class Shader;
	friend class OpenGLShader;
	friend class ShaderStruct;

public:
	virtual ~ShaderUniformDeclaration() = default;

	virtual const std::string &GetName() const = 0;
	virtual Uint32 GetSize() const = 0;
	virtual Uint32 GetCount() const = 0;
	virtual Uint32 GetOffset() const = 0;
	virtual ShaderDomain GetDomain() const = 0;

protected:
	virtual void SetOffset(Uint32 offset) = 0;
};

/////////////////////////////////////////
/// Shader Uniform Buffer Declaration ///
/////////////////////////////////////////
class ShaderUniformBufferDeclaration : public RefCounted
{
public:
	using List = std::vector<ShaderUniformBufferDeclaration *>;

public:
	virtual const std::string &GetName() const = 0;
	virtual Uint32 GetRegister() const = 0;
	virtual Uint32 GetSize() const = 0;
	virtual const ShaderUniformDeclaration::List &GetUniformDeclarations() const = 0;

	virtual ShaderUniformDeclaration *FindUniform(const std::string &name) = 0;
};


/////////////////////
/// Shader struct ///
/////////////////////
class ShaderStruct
{
public:
	using List = std::vector<ShaderStruct *>;

private:
	friend class Shader;

private:
	std::string m_Name;
	std::vector<ShaderUniformDeclaration *> m_Fields;
	Uint32 m_Size;
	Uint32 m_Offset;

public:
	explicit ShaderStruct(std::string name);

	void AddField(ShaderUniformDeclaration *field);

	const std::string &GetName() const { return m_Name; }
	Uint32 GetSize() const { return m_Size; }
	Uint32 GetOffset() const { return m_Offset; }
	const std::vector<ShaderUniformDeclaration *> &GetFields() const { return m_Fields; }

	void SetOffset(Uint32 offset) { m_Offset = offset; }
};

///////////////////////////////////
/// Shader Resource Declaration ///
///////////////////////////////////
class ShaderResourceDeclaration
{
public:
	using List = std::vector<ShaderResourceDeclaration *>;

public:
	virtual ~ShaderResourceDeclaration() = default;

	virtual const std::string &GetName() const = 0;
	virtual Uint32 GetRegister() const = 0;
	virtual Uint32 GetCount() const = 0;
};

}

