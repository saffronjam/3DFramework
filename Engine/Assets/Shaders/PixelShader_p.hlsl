struct PsInput
{
	float4 Position : SV_POSITION;
};

struct PS_OUTPUT
{
	float4 Color : SV_TARGET0;
};

PS_OUTPUT main(PsInput input)
{
	PS_OUTPUT output;

	output.Color = float4(1.0f, 0.0f, 1.0f, 1.0f);

	return output;
}
