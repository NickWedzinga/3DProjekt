#pragma pack_matrix(row_major)

cbuffer lights : register (b0)
{
	matrix View;
	matrix Proj;
	float3 noLights;
	float3 position;
	float3 intensity;
	float3 color;
	float3 direction;
};

cbuffer CONSTANT_BUFFER : register(b1)
{
	matrix WorldMatrix;
	matrix ViewMatrix;
	matrix ProjMatrix;
	float3 camDirection;
	float3 camPos;
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
	float4 pos : POSITION;
	float4 svPos : SV_POSITION;
	int ID : ID;
};

VS_Out VS_main(VS_IN input)
{
	VS_Out outPut;

	float3 poss = input.pos;
	//poss = mul(WorldMatrix, poss);
	poss = mul(View, poss);
	poss = mul(Proj, poss);
	outPut.pos = float4(poss, 1.0f);
	outPut.svPos = float4(poss, 1.0f);
	outPut.normal = input.normal;
	outPut.UV = input.UV;
	outPut.ID = input.ID;
	return outPut;
}