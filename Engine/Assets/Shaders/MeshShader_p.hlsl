struct PS_INPUT
{
	float4 Position : SV_POSITION;
	float3 Normal : Normal;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	return float4(input.Normal, 1.0f);
}
