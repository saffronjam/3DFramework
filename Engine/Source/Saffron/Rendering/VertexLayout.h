#pragma once

#include "Saffron/Base.h"
#include "Saffron/Rendering/GraphicsResource.h"

namespace Se
{
enum class VertexAttribute
{
	Position,
	//!< a_position
	Normal,
	//!< a_normal
	Tangent,
	//!< a_tangent
	Bitangent,
	//!< a_bitangent
	Color0,
	//!< a_color0
	Color1,
	//!< a_color1
	Color2,
	//!< a_color2
	Color3,
	//!< a_color3
	Indices,
	//!< a_indices
	Weight,
	//!< a_weight
	TexCoord0,
	//!< a_texcoord0
	TexCoord1,
	//!< a_texcoord1
	TexCoord2,
	//!< a_texcoord2
	TexCoord3,
	//!< a_texcoord3
	TexCoord4,
	//!< a_texcoord4
	TexCoord5,
	//!< a_texcoord5
	TexCoord6,
	//!< a_texcoord6
	TexCoord7,
	//!< a_texcoord7

	Count
};

enum class VertexAttributeType
{
	Uint8,
	//!< Uint8
	Uint10,
	//!< Uint10, availability depends on: `BGFX_CAPS_VERTEX_ATTRIB_UINT10`.
	Int16,
	//!< Int16
	Half,
	//!< Half, availability depends on: `BGFX_CAPS_VERTEX_ATTRIB_HALF`.
	Float,
	//!< Float

	Count
};

class VertexLayout : public GraphicsResource
{
public:
	VertexLayout& Begin();
	VertexLayout& Add(VertexAttribute attr, Uint8 count, VertexAttributeType type, bool normalized = false);
	VertexLayout& End();

	const bgfx::VertexLayout& GetNativeLayout() const;

private:
	static bgfx::Attrib::Enum SaffronToBgfxVertexAttribute(VertexAttribute vertexAttribute);
	static bgfx::AttribType::Enum SaffronToBgfxVertexAttributeType(VertexAttributeType vertexAttribute);

private:
	bgfx::VertexLayout _nativeVertexLayout;
};
}
