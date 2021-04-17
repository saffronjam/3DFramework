#include "SaffronPCH.h"

#include "Saffron/Rendering/Resources/ShaderUniform.h"

namespace Se
{
ShaderStruct::ShaderStruct(String name) :
	_name(Move(name)),
	_size(0),
	_offset(0)
{
}

void ShaderStruct::AddField(ShaderUniformDeclaration* field)
{
	_size += field->GetSize();
	Uint32 offset = 0;
	if (_fields.size())
	{
		ShaderUniformDeclaration* previous = _fields.back();
		offset = previous->GetOffset() + previous->GetSize();
	}
	field->SetOffset(offset);
	_fields.push_back(field);
}

const String& ShaderStruct::GetName() const
{
	return _name;
}

Uint32 ShaderStruct::GetSize() const
{
	return _size;
}

Uint32 ShaderStruct::GetOffset() const
{
	return _offset;
}

const ArrayList<ShaderUniformDeclaration*>& ShaderStruct::GetFields() const
{
	return _fields;
}

void ShaderStruct::SetOffset(Uint32 offset)
{
	_offset = offset;
}
}
