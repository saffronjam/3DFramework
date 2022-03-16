#include "Structs_i.hlsl"

cbuffer ShadowCommon : register(b0)
{
	float4 Values;
	matrix Model;
	PointLight Light;
}
