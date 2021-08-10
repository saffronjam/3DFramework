
cbuffer MvpBuffer : register(b0) { matrix Model; matrix Vp; matrix Mvp; }

#define MAX_LIGHTS 4

struct PointLight
{
	float3 Position;
};

cbuffer PointLights : register(b1)
{
	PointLight PointLights[MAX_LIGHTS];
	int nPointLights;
}