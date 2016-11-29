cbuffer lights : register (b0)
{
	float3 noLights;
	float3 position;
	float3 intensity;
	float3 color;
	float3 direction;
	matrix View;
	matrix Proj;
};

struct VS_IN
{
	float2 UV : UV;
	float3 normal : NORMAL;
	float3 pos : POSITION;
	int ID : ID;
};

float4  VS_main(VS_IN input) : SV_POSITION
{
	float3 pos = input.pos;
	pos = mul(View, pos);
	pos = mul(Proj, pos);
	return float4(pos, 1.0f);
}