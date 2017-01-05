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

struct VS_Out
{
	float2 UV : UV;
	float3 normal : NORMAL;
	float4 svPos : SV_POSITION;
	int ID : ID;
};

VS_Out VS_main(VS_IN input)
{
	VS_Out outPut;

	float4 poss = float4(input.pos, 1.0f);
	poss = mul(View, poss);
	poss = mul(Proj, poss);
	outPut.svPos = poss;
	outPut.normal = input.normal;
	outPut.UV = input.UV;
	outPut.ID = input.ID;
	return outPut;
}

float4 main(VS_IN input) : SV_POSITION
{
	float4 temp = float4(input.pos, 1.0f);
	temp = mul(View, temp);
	temp = mul(Proj, temp);
	return temp;
}