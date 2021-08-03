cbuffer MvpBuffer : register(b0) { matrix Model; matrix View; matrix Projection;}

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
};

VS_OUTPUT main(
	float3 position : Position,
	float3 normal : Normal,
	float3 tangent : Tangent,
	float3 binormal : Binormal,
	float2 texCoord : TexCoord
)
{
	VS_OUTPUT output;

	float4 pos4 = float4(position, 1.0);
	pos4 = mul(pos4, Model);
	pos4 = mul(pos4, View);
	pos4 = mul(pos4, Projection);
	output.Position = pos4;

	return output;
}
