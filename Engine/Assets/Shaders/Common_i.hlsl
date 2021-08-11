
cbuffer MvpBuffer : register(b0) { matrix Model; matrix Mv; matrix Vp; matrix Mvp; }

#define MAX_LIGHTS 4

struct PointLight
{
	float3 Position;
};

cbuffer CommonCBuffer : register(b1)
{
	float3 CameraPosition;
}

cbuffer PointLights : register(b2)
{
	PointLight PointLights[MAX_LIGHTS];
	int nPointLights;
}