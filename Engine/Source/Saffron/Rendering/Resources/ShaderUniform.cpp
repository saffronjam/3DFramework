#include "SaffronPCH.h"

#include "Saffron/Rendering/Resources/ShaderUniform.h"

namespace Se
{
ShaderStruct::ShaderStruct(String name) :
	m_Name(Move(name)),
	m_Size(0),
	m_Offset(0)
{
}

void ShaderStruct::AddField(ShaderUniformDeclaration* field)
{
	m_Size += field->GetSize();
	Uint32 offset = 0;
	if (m_Fields.size())
	{
		ShaderUniformDeclaration* previous = m_Fields.back();
		offset = previous->GetOffset() + previous->GetSize();
	}
	field->SetOffset(offset);
	m_Fields.push_back(field);
}

const String& ShaderStruct::GetName() const
{
	return m_Name;
}

Uint32 ShaderStruct::GetSize() const
{
	return m_Size;
}

Uint32 ShaderStruct::GetOffset() const
{
	return m_Offset;
}

const ArrayList<ShaderUniformDeclaration*>& ShaderStruct::GetFields() const
{
	return m_Fields;
}

void ShaderStruct::SetOffset(Uint32 offset)
{
	m_Offset = offset;
}
}
