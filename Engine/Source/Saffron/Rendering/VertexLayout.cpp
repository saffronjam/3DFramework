#include "SaffronPCH.h"

#include "Saffron/Rendering/VertexLayout.h"

namespace Se
{
VertexLayout& VertexLayout::Begin()
{
	_nativeVertexLayout.begin();
	return *this;
}

VertexLayout& VertexLayout::Add(VertexAttribute attr, Uint8 count, VertexAttributeType type, bool normalized )
{
	_nativeVertexLayout.add(SaffronToBgfxVertexAttribute(attr), count, SaffronToBgfxVertexAttributeType(type), normalized);
	return *this;
}

VertexLayout& VertexLayout::End()
{
	_nativeVertexLayout.end();
	return *this;
}

const bgfx::VertexLayout& VertexLayout::GetNativeLayout() const
{
	return _nativeVertexLayout;
}

bgfx::Attrib::Enum VertexLayout::SaffronToBgfxVertexAttribute(VertexAttribute vertexAttribute)
{
	switch (vertexAttribute)
	{
	case VertexAttribute::Position: return bgfx::Attrib::Enum::Position;
	case VertexAttribute::Normal: return bgfx::Attrib::Enum::Normal;
	case VertexAttribute::Tangent: return bgfx::Attrib::Enum::Tangent;
	case VertexAttribute::Bitangent: return bgfx::Attrib::Enum::Bitangent;
	case VertexAttribute::Color0: return bgfx::Attrib::Enum::Color0;
	case VertexAttribute::Color1: return bgfx::Attrib::Enum::Color1;
	case VertexAttribute::Color2: return bgfx::Attrib::Enum::Color2;
	case VertexAttribute::Color3: return bgfx::Attrib::Enum::Color3;
	case VertexAttribute::Indices: return bgfx::Attrib::Enum::Indices;
	case VertexAttribute::Weight: return bgfx::Attrib::Enum::Weight;
	case VertexAttribute::TexCoord0: return bgfx::Attrib::Enum::TexCoord0;
	case VertexAttribute::TexCoord1: return bgfx::Attrib::Enum::TexCoord1;
	case VertexAttribute::TexCoord2: return bgfx::Attrib::Enum::TexCoord2;
	case VertexAttribute::TexCoord3: return bgfx::Attrib::Enum::TexCoord3;
	case VertexAttribute::TexCoord4: return bgfx::Attrib::Enum::TexCoord4;
	case VertexAttribute::TexCoord5: return bgfx::Attrib::Enum::TexCoord5;
	case VertexAttribute::TexCoord6: return bgfx::Attrib::Enum::TexCoord6;
	case VertexAttribute::TexCoord7: return bgfx::Attrib::Enum::TexCoord7;
	case VertexAttribute::Count: return bgfx::Attrib::Enum::Count;
	default: SE_CORE_FALSE_ASSERT("Invalid vertex attribute in conversion")
		return static_cast<bgfx::Attrib::Enum>(-1);
	}
}

bgfx::AttribType::Enum Se::VertexLayout::SaffronToBgfxVertexAttributeType(VertexAttributeType vertexAttribute)
{
	switch (vertexAttribute)
	{
	case VertexAttributeType::Uint8: return bgfx::AttribType::Enum::Uint8;
	case VertexAttributeType::Uint10: return bgfx::AttribType::Enum::Uint10;
	case VertexAttributeType::Int16: return bgfx::AttribType::Enum::Int16;
	case VertexAttributeType::Half: return bgfx::AttribType::Enum::Half;
	case VertexAttributeType::Float: return bgfx::AttribType::Enum::Float;
	case VertexAttributeType::Count: return bgfx::AttribType::Enum::Count;
	default: SE_CORE_FALSE_ASSERT("Invalid vertex attribute type in conversion")
		return static_cast<bgfx::AttribType::Enum>(-1);
	}
}
}
