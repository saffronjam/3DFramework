struct PsInput
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR0;
};

float4 main(PsInput input) : SV_TARGET
{
	return input.Color;
}
