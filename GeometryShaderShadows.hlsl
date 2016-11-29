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

cbuffer CONSTANT_BUFFER : register (b1)
{
	matrix WorldMatrix;
	matrix ViewMatrix;
	matrix ProjMatrix;
	float3 camDirection;
	float3 camPos;
};

struct GS_IN
{
	float2 UV : UV;
	float3 normal : NORMAL;
	float4 pos : SV_POSITION;
	float ID : ID;
};

struct GS_OUT
{
	float4 pos : SV_POSITION;
	float3 light : LIGHT;
	/*float3 light[5] : LIGHT;*/
};

[maxvertexcount(36)]
void GS_main(triangle GS_IN input[3], inout TriangleStream< GS_OUT > output)
{
	GS_OUT element;
	for (uint i = 0; i < 3; i++)
	{
		//for (uint j = 0; j < noLights.x; j++)
		//{
		//	float4 tempPos = input[i].pos;
		//	tempPos = mul(View, tempPos);
		//	tempPos = mul(Proj, tempPos);
		//	element.light = tempPos.xyz;
		//	element.pos = tempPos;

		//	output.Append(element);
		//}
		float4 tempPos = input[i].pos;
		/*tempPos = mul(View, tempPos);
		tempPos = mul(Proj, tempPos);*/
		tempPos = mul(WorldMatrix, tempPos);
		tempPos = mul(ViewMatrix, tempPos);
		tempPos = mul(ProjMatrix, tempPos);
		element.light = tempPos.xyz;
		element.pos = tempPos;

		output.Append(element);
	}
}