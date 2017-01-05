cbuffer lights : register (b0)
{
	float3 lightPosition;
	float3 intensity;
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

float4 main(VS_IN input) : SV_POSITION
{
	float4 temp = float4(input.pos, 1.0f);
	temp = mul(View, temp);
	temp = mul(Proj, temp);
	return temp;
}