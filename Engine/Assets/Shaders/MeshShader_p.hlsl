Texture2D ShaderTexture;
SamplerState SampleType;

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float3 normal : Normal;
	float2 texCoord : TexCoord;
	float3 tangent : Tangent;
	float3 binormal : Binormal;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	//return float4(input.tangent, 1.0f);
	return ShaderTexture.Sample(SampleType, input.texCoord);
}
