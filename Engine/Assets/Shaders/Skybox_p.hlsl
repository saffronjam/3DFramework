TextureCube tex : register(t0);
SamplerState sam : register(s0);


float4 main(float3 WorldPos : Position) : SV_TARGET
{
    float4 sample = tex.Sample(sam, WorldPos.xyz);

    float3 envColor = sample.xyz;

	envColor = envColor / (envColor + float3(1.0, 1.0, 1.0));
	envColor = pow(envColor, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));

    return sample;
    float4(envColor, 1.0);
}
