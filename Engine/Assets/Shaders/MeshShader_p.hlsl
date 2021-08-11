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
	float3 WorldPosition : WorldPosition;
	float3 ViewPos : ViewPos;

	float3 LightPos[MAX_LIGHTS] : LightPos;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	const float3 normal = normalize(input.Normal);
	const float3 lightColor = float3(1.0f, 0.0f, 0.0f);

	// Ambient
	const float3 ambientStength = 0.1f;
	const float3 ambient = ambientStength * float3(1.0f, 1.0f, 1.0f);


	// Diffuse
	const float3 lightDir = normalize(input.LightPos[0] - input.WorldPosition);
	const float diffuseStrength = max(dot(normal, lightDir), 0.0f);
	const float3 diffuse = diffuseStrength * lightColor;

	// Specular
	const float specularStrength = 0.5;
	const float3 viewDir = normalize(input.ViewPos - input.WorldPosition);
	const float3 reflectDir = reflect(-lightDir, normal);
	const float specularIntensity = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
	const float3 specular = specularStrength * specularIntensity * lightColor;

	
	const float4 objColor = ShaderTexture.Sample(SampleType, input.TexCoord);
	const float4 finalColor = float4(ambient + diffuse + specular, 1.0f);

	return finalColor;
}
