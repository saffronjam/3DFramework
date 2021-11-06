#include "Common_i.hlsl"


const float PiLocal = 3.141592f;
#define ModelTextureMapType_Albedo 0
#define ModelTextureMapType_Normal 1
#define ModelTextureMapType_Roughness 2
#define ModelTextureMapType_Metalness 3
#define ModelTextureMapType_Count 4

Texture2D ModelTex[ModelTextureMapType_Count] : register(t0);
SamplerState ModelTexSam : register(s0);

Texture2D ShadowMapTexture : register(t4);
SamplerState ShadowMapSampleType : register(s4);


///////////////
///// PBR /////
///////////////

cbuffer PBR_Material : register(b3){ float4 AlbedoColor; float Metalness; float Roughness; float Emission; float
EnvMapRotation; bool UseNormalMap;};

struct PBR_Data
{
	float3 Albedo;
	float Roughness;
	float Metalness;

	float3 Normal;
	float3 ViewDir;
};

static const float3 FrenselDielectric = 0.04f;


// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2.
float ndfGGX(float cosLh, float roughness)
{
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;

    float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
    return alphaSq / (Pi * denom * denom);
}

// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
    return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
    return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

// Shlick's approximation of the Fresnel factor.
float3 fresnelSchlick(float3 F0, float cosTheta)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 fresnelSchlickRoughness(float3 F0, float cosTheta, float roughness)
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

	if (ndcPosition.z >= 1.0f || ndcPosition.z < 0.0f)
        return 1.0f;

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

float3 CalculatePointLights(float3 F0, in PBR_Data pbrData, float3 pixelWorldPosition)
{
    float3 sum = float3(0.0, 0.0, 0.0);
    for (int i = 0; i < 1; i++)
    {
        PointLight light = PointLights[i];
        
        float3 Li = normalize(light.Position - pixelWorldPosition);
        float3 Lh = normalize(Li + pbrData.ViewDir);
        
        const float Ldistance = length(light.Position - pixelWorldPosition);
        float Lattenuation = clamp(1.0 - (Ldistance * Ldistance) / (light.Radius * light.Radius), 0.0, 1.0);
        Lattenuation *= lerp(Lattenuation, 1.0, 0.0f /*Falloff*/);
        const float3 Lradiance = light.Color * Lattenuation;

        const float cosLi = max(0.0, dot(pbrData.Normal, Li));
        const float cosLh = max(0.0, dot(pbrData.Normal, Lh));
        const float cosLo = max(dot(pbrData.Normal, pbrData.ViewDir), 0.0);

        const float3 F = fresnelSchlickRoughness(F0, max(dot(Lh, pbrData.ViewDir), 0.0), pbrData.Roughness);
        const float D = ndfGGX(cosLh, pbrData.Roughness);
        const float G = gaSchlickGGX(cosLi, cosLo, pbrData.Roughness);

        const float3 kD = (1 - F) * (1.0 - pbrData.Metalness);
        const float3 specular = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);

        sum += (kD * pbrData.Albedo + specular) * Lradiance * cosLi;
    }

    float3 gammaCorr = pow(sum / (sum + float3(1.0, 1.0, 1.0)), float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));

    return gammaCorr;
}

const float2 invAtan = float2(0.1591, 0.3183);
float2 SampleSphericalMap(float3 v)
{
    float2 uv = float2(atan2(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

float4 main(PsInput input) : SV_TARGET
{
	//// PBR
	PBR_Data _pbrData;

	// Calculate PBR inputs (one of each pair are 1.0f, hence why they can be multiplied)
	_pbrData.Albedo = ModelTex[ModelTextureMapType_Albedo].Sample(ModelTexSam, input.TexCoord).rgb * AlbedoColor.xyz;
    _pbrData.Metalness = ModelTex[ModelTextureMapType_Roughness].Sample(ModelTexSam, input.TexCoord).b * Metalness;
	_pbrData.Roughness = ModelTex[ModelTextureMapType_Roughness].Sample(ModelTexSam, input.TexCoord).g * Roughness;
	_pbrData.Roughness = max(_pbrData.Roughness, 0.05f);

    float ao = ModelTex[ModelTextureMapType_Roughness].Sample(ModelTexSam, input.TexCoord).r;

	// Normals can't be multiplied, either value or map
	if (UseNormalMap)
    {
		_pbrData.Normal = normalize(
			ModelTex[ModelTextureMapType_Normal].Sample(ModelTexSam, input.TexCoord).rgb * 2.0f - 1.0f
		);
        _pbrData.Normal = normalize(mul(input.WorldNormals, _pbrData.Normal));
    }
	else
	{
		_pbrData.Normal = normalize(input.Normal);
    }
	_pbrData.ViewDir = normalize(input.ViewPos - input.WorldPosition);				// Lo

	// Frensel reflectance
    float3 F0 = lerp(float3(0.04, 0.04, 0.04), pow(_pbrData.Albedo, float3(2.2, 2.2, 2.2)), _pbrData.Metalness);

    float3 pointLightsContribution = CalculatePointLights(F0, _pbrData, input.WorldPosition);

    return float4(pointLightsContribution, 1.0f);
}
