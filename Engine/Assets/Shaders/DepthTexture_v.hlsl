cbuffer vertexBuffer : register(b0) { float4x4 ProjectionMatrix; };

struct VS_INPUT
{
	float2 pos : POSITION;
	float4 col : COLOR0;
	float2 uv : TEXCOORD0;
};

struct PsInput
{
	float4 pos : SV_POSITION;
	float4 col : COLOR0;
	float2 uv : TEXCOORD0;
};

PsInput main(VS_INPUT input)
{
	PsInput output;
	output.pos = mul(ProjectionMatrix, float4(input.pos.xy, 0.0f, 1.0f));
	output.col = input.col;
	output.uv = input.uv;
	return output;
}
