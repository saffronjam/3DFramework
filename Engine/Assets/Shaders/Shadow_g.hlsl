#include "Shadow_i.hlsl"

struct GsInput
{
	float4 Position : SV_POSITION;
};

struct GsOutput
{
	float4 Position : SV_POSITION;
	float4 PixPos : PixPos;
	uint RtIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount(18)]
void main(triangle GsInput input[3], inout TriangleStream<GsOutput> cubeMapStream)
{
	for (int face = 0; face < 6; ++face)
	{
		GsOutput output;
		output.RtIndex = face;

		for (int vertex = 0; vertex < 3; vertex++)
		{
			output.PixPos = input[vertex].Position;
			output.Position = mul(input[vertex].Position, Light.LookAt[face]);
			cubeMapStream.Append(output);
		}
		cubeMapStream.RestartStrip();
	}
}
