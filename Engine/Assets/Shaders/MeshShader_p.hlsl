#include "Common_i.hlsl"

Texture2D ShaderTexture;
SamplerState SampleType;

struct PS_INPUT
{
	float4 Position : SV_POSITION;
	float3 Normal : Normal;
	float2 TexCoord : TexCoord;
	float3 Tangent : Tangent;
	float3 Binormal : Binormal;

	float3 LightPos[MAX_LIGHTS] : LightPos;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	const float3 lightDir = input.LightPos[0] - input.Position;
	const float diffuseStrength = max(dot(input.Normal, lightDir), 0.0f);
	float3 diffuse = diffuseStrength * float3(1.0f, 0.4f, 0.4f);


	const float4 objColor = ShaderTexture.Sample(SampleType, input.TexCoord);
	const float4 finalColor = float4(diffuse, 0.0f) + objColor;
	
	return finalColor;
}
