#include "SaffronPCH.h"

#include "Saffron/Renderer/ShaderUniform.h"

namespace Se
{
ShaderStruct::ShaderStruct(String name)
	: m_Name(Move(name)), m_Size(0), m_Offset(0)
{
}

void ShaderStruct::AddField(ShaderUniformDeclaration *field)
{
	m_Size += field->GetSize();
	Uint32 offset = 0;
	if ( !m_Fields.empty() )
	{
		ShaderUniformDeclaration *previous = m_Fields.back();
		offset = previous->GetOffset() + previous->GetSize();
	}
	field->SetOffset(offset);
	m_Fields.push_back(field);
}
}
