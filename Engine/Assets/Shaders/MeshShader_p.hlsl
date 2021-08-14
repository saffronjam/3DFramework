#include "Common_i.hlsl"

Texture2D MeshTexture : register(t0);
SamplerState MeshSampleType : register(s0);

Texture2D ShadowMapTexture : register(t1);
SamplerState ShadowMapSampleType : register(s1);

float CalculateShadow(float4 lightSpacePosition)
{
	// Perspective divide because LightSpacePosition is not passed through as "SV_POSITION"
	float3 ndcPosition = (lightSpacePosition.xyz / lightSpacePosition.w);

	if (ndcPosition.z > 1.0f) return 0.0f;

	ndcPosition.x *= 0.5f;
	ndcPosition.y *= 0.5f;

	ndcPosition.x += 0.5f;
	ndcPosition.y += 0.5f;

	const float closestDepth = ShadowMapTexture.Sample(ShadowMapSampleType, ndcPosition.xy).r;
	const float currentDepth = ndcPosition.z;

	// 1 -> full shadow. 0 -> no shadow
	return currentDepth > closestDepth ? 1.0f : 0.0f;
}

struct PsInput
{
	float4 Position : SV_POSITION;
	float3 WorldPosition : WorldPosition;

	float3 Normal : Normal;
	float2 TexCoord : TexCoord;
	float3 Tangent : Tangent;
	float3 Binormal : Binormal;

	float3 ViewPos : ViewPos;
	float3 LightPos[MAX_LIGHTS] : LightPos;
	float4 LightSpacePosition : LightSpacePosition;
};

float4 main(PsInput input) : SV_TARGET
{
	const float3 normal = normalize(input.Normal);
	const float3 lightColor = float3(1.0f, 0.0f, 0.0f);

	// Ambient
	const float3 ambientStength = 0.1f;
	const float3 ambient = ambientStength * float3(1.0f, 1.0f, 1.0f);

	// Diffuse
	const float3 lightDir = normalize(input.LightPos[0] - input.WorldPosition);
	const float diffuseStrength = max(dot(lightDir, normal), 0.0f);
	const float3 diffuse = diffuseStrength * lightColor;

	// Specular
	const float specularStrength = 0.5;
	const float3 viewDir = normalize(input.ViewPos - input.WorldPosition);
	float specularIntensity = 0.0;
	const float3 halfwayDir = normalize(lightDir + viewDir);
	specularIntensity = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
	const float3 specular = specularIntensity * lightColor;

	// Shadow
	const float shadowStrength = CalculateShadow(input.LightSpacePosition);
	

	const float near = 0.1f;
	const float far = 30.0f;

	float lineardepth = (2.0f * near) / (far + near - shadowStrength * (far - near));


	const float4 objColor = MeshTexture.Sample(ShadowMapSampleType, input.TexCoord);
	const float4 finalColor = float4(ambient + (1.0f - shadowStrength) * (diffuse + specular), 1.0f);


	return finalColor;
}
