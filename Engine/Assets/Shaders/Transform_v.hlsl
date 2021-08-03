cbuffer MvpBuffer : register(b0) { matrix Model; matrix View; matrix Projection;}

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
};

static matrix Identity = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};

VS_OUTPUT main(float3 pos : Position)
{
	VS_OUTPUT output;

	float4 pos4 = float4(pos, 1.0);
	pos4 = mul(pos4, Model);
	pos4 = mul(pos4, View);
	pos4 = mul(pos4, Projection);
	output.Position = pos4;

	return output;
}
