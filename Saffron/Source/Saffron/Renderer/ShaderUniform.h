#pragma once




#include "Saffron/Base.h"

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
	using List = ArrayList<ShaderUniformDeclaration *>;

private:
	friend class Shader;
	friend class OpenGLShader;
	friend class ShaderStruct;

public:
	virtual ~ShaderUniformDeclaration() = default;

	virtual const String &GetName() const = 0;
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
class ShaderUniformBufferDeclaration : public MemManaged<ShaderUniformBufferDeclaration>
{
public:
	using List = ArrayList<ShaderUniformBufferDeclaration *>;

public:
	virtual const String &GetName() const = 0;
	virtual Uint32 GetRegister() const = 0;
	virtual Uint32 GetSize() const = 0;
	virtual const ShaderUniformDeclaration::List &GetUniformDeclarations() const = 0;

	virtual ShaderUniformDeclaration *FindUniform(const String &name) = 0;
};


/////////////////////
/// Shader struct ///
/////////////////////
class ShaderStruct
{
public:
	using List = ArrayList<ShaderStruct *>;

private:
	friend class Shader;

private:
	String m_Name;
	ArrayList<ShaderUniformDeclaration *> m_Fields;
	Uint32 m_Size;
	Uint32 m_Offset;

public:
	explicit ShaderStruct(String name);

	void AddField(ShaderUniformDeclaration *field);

	const String &GetName() const { return m_Name; }
	Uint32 GetSize() const { return m_Size; }
	Uint32 GetOffset() const { return m_Offset; }
	const ArrayList<ShaderUniformDeclaration *> &GetFields() const { return m_Fields; }

	void SetOffset(Uint32 offset) { m_Offset = offset; }
};

///////////////////////////////////
/// Shader Resource Declaration ///
///////////////////////////////////
class ShaderResourceDeclaration
{
public:
	using List = ArrayList<ShaderResourceDeclaration *>;

public:
	virtual ~ShaderResourceDeclaration() = default;

	virtual const String &GetName() const = 0;
	virtual Uint32 GetRegister() const = 0;
	virtual Uint32 GetCount() const = 0;
};

}

