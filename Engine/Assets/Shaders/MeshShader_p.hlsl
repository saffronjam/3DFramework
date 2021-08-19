#include "Common_i.hlsl"

#define ModelTextureMapType_Albedo 0
#define ModelTextureMapType_Normal 1
#define ModelTextureMapType_Roughness 2
#define ModelTextureMapType_Metalness 3
#define ModelTextureMapType_Count 4

Texture2D ModelTex[ModelTextureMapType_Count] : register(t0);
SamplerState ModelTexSam : register(s0);

Texture2D ShadowMapTexture : register(t4);
SamplerState ShadowMapSampleType : register(s4);


/////////
// PBR //
/////////

cbuffer PBR_Material : register(b3){ float4 AlbedoColor; float Metalness; float Roughness; float Emission; float
EnvMapRotation; bool UseNormalMap;};

struct PBR_Data
{
	float3 Albedo;
	float Roughness;
	float Metalness;

	float3 Normal;
	float3 ViewDir;
	float NdotV;
};

const float FrenselDielectric = float3(0.04f, 0.04f, 0.04f);

float DistributionGGX(float NdotH, float roughness)
{
	const float alpha = roughness * roughness;
	const float alphaSq = alpha * alpha;

	float denom = NdotH * NdotH * (alphaSq - 1.0f) + 1.0f;
	denom = Pi * denom * denom;

	return alphaSq / max(denom, 0.0000001);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	const float r = roughness + 1.0f;
	const float k = r * r / 8.0f;

	const float num = NdotV;
	const float denom = NdotV * (1.0 - k) + k;

	return num / denom;
}

float GeometrySmith(float3 normal, float3 view, float3 light, float roughness)
{
	const float NdotV = max(dot(normal, view), 0.0);
	const float NdotL = max(dot(normal, light), 0.0);
	const float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	const float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

float gaSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

float gaSchlickGGX(float cosLi, float NdotV, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
	return gaSchlickG1(cosLi, k) * gaSchlickG1(NdotV, k);
}

float3 FresnelSchlick(float3 F0, float cosTheta)
{
	return F0 + (1.0f - F0) * pow(max(1.0f - cosTheta, 0.0f), 5.0f);
}

float3 FresnelSchlickRoughness(float3 F0, float cosTheta, float roughness)
{
	return F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}


///////////////
/// Shadows ///
///////////////


float CalculateShadow(float4 lightSpacePosition)
{
	// Perspective divide because LightSpacePosition is not passed through as "SV_POSITION"
	float3 ndcPosition = (lightSpacePosition.xyz / lightSpacePosition.w);

	if (ndcPosition.z > 1.0f) return 1.0f;

	ndcPosition.x *= 0.5f;
	ndcPosition.y *= -0.5f;

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

	float3x3 WorldNormals : WorldNormals;
};

float4 main(PsInput input) : SV_TARGET
{
	//// PBR
	PBR_Data _pbrData;

	// Calculate PBR inputs (one of each pair are 1.0f, hence why they can be multiplied)
	_pbrData.Albedo = ModelTex[ModelTextureMapType_Albedo].Sample(ModelTexSam, input.TexCoord).rgb * AlbedoColor.xyz;
	_pbrData.Metalness = ModelTex[ModelTextureMapType_Metalness].Sample(ModelTexSam, input.TexCoord).r * Metalness;
	_pbrData.Roughness = ModelTex[ModelTextureMapType_Roughness].Sample(ModelTexSam, input.TexCoord).r * Roughness;
	_pbrData.Roughness = max(_pbrData.Roughness, 0.05f);

	// Normals can't be multiplied, either value or map
	if (UseNormalMap)
	{
		_pbrData.Normal = normalize(
			input.Normal + ModelTex[ModelTextureMapType_Normal].Sample(ModelTexSam, input.TexCoord).rgb
		);
	}
	else
	{
		_pbrData.Normal = normalize(input.Normal);
	}

	_pbrData.ViewDir = normalize(input.ViewPos - input.WorldPosition);

	// Frensel reflectance
	const float3 F0 = lerp(FrenselDielectric, _pbrData.Albedo, _pbrData.Metalness);

	const float3 lightDir = normalize(input.LightPos[0] - input.WorldPosition);
	const float3 halfwayDir = normalize(lightDir + _pbrData.ViewDir);
	const float lightDistance = length(input.LightPos[0] - input.WorldPosition);
	const float attenuation = 1.0f / (lightDistance * lightDistance);
	const float3 lightColor = float3(1.0f, 1.0f, 1.0f);
	const float3 lightRadiance = lightColor * attenuation;

	const float NdotV = max(0.0f, dot(_pbrData.Normal, _pbrData.ViewDir));
	const float NdotL = max(0.0f, dot(_pbrData.Normal, lightDir));
	const float NdotH = max(0.0f, dot(_pbrData.Normal, halfwayDir));
	const float HdotV = max(0.0f, dot(_pbrData.ViewDir, halfwayDir));


	// Cook-Torrance BRDF
	const float3 F = FresnelSchlickRoughness(F0, HdotV, _pbrData.Roughness);
	const float D = DistributionGGX(NdotH, _pbrData.Roughness);
	const float G = gaSchlickGGX(NdotL, NdotV, _pbrData.Roughness);

	// Diffuse
	const float kD = (float3(1.0f, 1.0f, 1.0f) - F) * (1.0f - _pbrData.Metalness);
	const float diffuse = kD * _pbrData.Albedo;

	// Specular
	float3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * NdotL * NdotV);
	specularBRDF = clamp(specularBRDF, float3(0.0f, 0.0f, 0.0f), float3(10.0f, 10.0f, 10.0f));
	float3 result = (diffuse + specularBRDF) * lightRadiance * NdotL * _pbrData.Albedo;
	result += _pbrData.Albedo * Emission;

	const float shadow = CalculateShadow(input.LightSpacePosition);
	const float ambient = float3(0.03f, 0.03f, 0.03f) * _pbrData.Albedo;


	const float3 finalColor = result * (1.0f - shadow) + ambient;


	return float4(finalColor, 1.0f);
}
