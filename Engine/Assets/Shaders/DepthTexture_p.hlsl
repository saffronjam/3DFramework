struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 col : COLOR0;
	float2 uv : TEXCOORD0;
};

sampler sampler0;
Texture2D texture0;

const float near_plane = 0.1f;
const float far_plane = 10.0f;

float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0; // Back to NDC 
	return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

float4 main(PS_INPUT input) : SV_Target
{
	float4 out_col = input.col * texture0.Sample(sampler0, input.uv);

	const float near = 0.1f;
	const float far = 30.0f;
	const float lineardepth = (2.0f * near) / (far + near - out_col.r * (far - near));
	return out_col.g = out_col.b = out_col.r = lineardepth;
}
