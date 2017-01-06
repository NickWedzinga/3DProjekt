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

struct VS_OUT
{
	float2 UV : UV;
	float4 normal : NORMAL;
	float4 pos : POSITION;
	int ID : ID;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output;

	output.UV = input.UV;
	output.normal = float4(input.normal, 0);
	float4 temp = float4(input.pos, 1);
	temp = mul(View, temp);
	temp = mul(Proj, temp);
	output.pos = temp;
	output.ID = input.ID;

	return output;
}

float4 main(VS_IN input) : SV_POSITION
{
	float4 temp = float4(input.pos, 1.0f);
	temp = mul(View, temp);
	temp = mul(Proj, temp);
	return temp;
}