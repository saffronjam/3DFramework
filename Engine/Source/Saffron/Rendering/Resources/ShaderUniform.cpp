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
	uint offset = 0;
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

uint ShaderStruct::GetSize() const
{
	return _size;
}

uint ShaderStruct::GetOffset() const
{
	return _offset;
}

const List<ShaderUniformDeclaration*>& ShaderStruct::GetFields() const
{
	return _fields;
}

void ShaderStruct::SetOffset(uint offset)
{
	_offset = offset;
}
}
