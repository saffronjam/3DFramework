#include "Common_i.hlsl"


Texture2D equirectangularTex : register(t0);
RWTexture2DArray<float> cubemap : register(u0);

SamplerState equirectSampler : register(s0);

float3 GetCubeMapTexCoord(uint3 DTid)
{
    float width;
    float height;
    float elements;
    cubemap.GetDimensions(width, height, elements);

    float2 st = DTid.xy / float2(width, height);
    float2 uv = 2.0 * float2(st.x, 1.0 - st.y) - float2(1.0, 1.0);

    float3 ret;
    if (DTid.z == 0)      ret = float3(1.0, uv.y, -uv.x);
    else if (DTid.z == 1) ret = float3(-1.0, uv.y, uv.x);
    else if (DTid.z == 2) ret = float3(uv.x, 1.0, -uv.y);
    else if (DTid.z == 3) ret = float3(uv.x, -1.0, uv.y);
    else if (DTid.z == 4) ret = float3(uv.x, uv.y, 1.0);
    else if (DTid.z == 5) ret = float3(-uv.x, uv.y, -1.0);
    return normalize(ret);
}

[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float3 cubeTC = GetCubeMapTexCoord(DTid);

    // Calculate sampling coords for equirectangular texture
	// https://en.wikipedia.org/wiki/Spherical_coordinate_system#Cartesian_coordinates
    float phi = atan2(cubeTC.z, cubeTC.x);
    float theta = acos(cubeTC.y);
    float2 uv = float2(phi / (2.0 * Pi) + 0.5, theta / Pi);

    float4 color = { 0.0, 0.0, 0.0, 0.0 };
    cubemap[DTid] = color;
}