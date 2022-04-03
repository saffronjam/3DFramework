cbuffer ShadowCommon : register(b0)
{
	float4 Values;
	matrix Model;
	matrix Mvp;
	float3 Position;
	float Radius;
}
