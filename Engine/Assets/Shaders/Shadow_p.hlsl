#include "Shadow_i.hlsl"

struct PsInput
{
	float4 Position : SV_Position;
	float4 PixPos : PixPos;
};

struct PsOutput
{
	float4 Value : SV_Target;
	float Depth : SV_Depth;
};

PsOutput main(PsInput input)
{
	float lightDistance = length(input.PixPos.xyz - Position);
	lightDistance = lightDistance / Radius;

	PsOutput output;

	output.Value = float4(1, 0, 1, 1);
	output.Depth = lightDistance;

	return output;
}
