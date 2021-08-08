cbuffer MvpBuffer : register(b0) { matrix Model; matrix Vp; matrix Mvp; }

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 normal : Normal;
	float2 texCoord : TexCoord;
	float3 tangent : Tangent;
	float3 binormal : Binormal;
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
	pos4 = mul(pos4, Mvp);
	output.position = pos4;
	output.normal = normal;
	output.texCoord = texCoord;
	output.tangent = tangent;
	output.binormal = binormal;
	
	return output;
}
