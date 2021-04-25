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
/// Shader UniformDeclaration ////
//////////////////////////////////

class ShaderUniformDeclaration
{
public:

private:
	friend class Shader;
	friend class OpenGLShader;
	friend class ShaderStruct;

public:
	virtual ~ShaderUniformDeclaration() = default;

	virtual const String& GetName() const = 0;
	virtual uint GetSize() const = 0;
	virtual uint GetCount() const = 0;
	virtual uint GetOffset() const = 0;
	virtual ShaderDomain GetDomain() const = 0;

protected:
	virtual void SetOffset(uint offset) = 0;
};

using ShaderUniformList = ArrayList<ShaderUniformDeclaration*>;

/////////////////////////////////////////
/// Shader Uniform Buffer Declaration ///
/////////////////////////////////////////

class ShaderUniformBufferDeclaration : public Managed
{
public:
	virtual const String& GetName() const = 0;
	virtual uint GetRegister() const = 0;
	virtual uint GetSize() const = 0;
	virtual const ShaderUniformList& GetUniformDeclarations() const = 0;

	virtual ShaderUniformDeclaration* FindUniform(const String& name) = 0;
};

using ShaderUniformBufferList = ArrayList<ShaderUniformBufferDeclaration*>;

/////////////////////
/// Shader struct ///
/////////////////////

class ShaderStruct
{
private:
	friend class Shader;

private:
	String _name;
	ArrayList<ShaderUniformDeclaration*> _fields;
	uint _size;
	uint _offset;

public:
	explicit ShaderStruct(String name);

	void AddField(ShaderUniformDeclaration* field);

	const String& GetName() const;
	uint GetSize() const;
	uint GetOffset() const;
	const ArrayList<ShaderUniformDeclaration*>& GetFields() const;
	void SetOffset(uint offset);
};

using ShaderStructList = ArrayList<ShaderStruct*>;

///////////////////////////////////
/// Shader Resource UniformDecl ///
///////////////////////////////////

class ShaderResourceDeclaration
{
public:
	virtual ~ShaderResourceDeclaration() = default;

	virtual const String& GetName() const = 0;
	virtual uint GetRegister() const = 0;
	virtual uint GetCount() const = 0;
};

using ShaderResourceList = ArrayList<ShaderResourceDeclaration*>;
}
