cbuffer CONSTANT_BUFFER : register(b0)
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
	float4 normal : NORMAL;
	float4 pos : SV_POSITION;
	float ID : ID;
};

struct GSOutput
{
	float2 UV : UV;
	float4 normal : NORMAL;
	float4 pos : SV_POSITION;
	float ID : ID;
};

[maxvertexcount(48)]
void GS_main(triangle GS_IN input[3], inout TriangleStream< GSOutput > output)
{
	GSOutput element;

	for (uint i = 0; i < 3; i++)
	{
		float4 poss = input[i].pos;
		poss = mul(WorldMatrix, poss);
		poss = mul(ViewMatrix, poss);
		poss = mul(ProjMatrix, poss);
		element.pos = poss;
		float4 normal = input[i].normal;
		normal = mul(WorldMatrix, normal);
		normal = mul(ViewMatrix, normal);
		normal = mul(ProjMatrix, normal);
		element.normal = normal;
		element.UV = input[i].UV;
		element.ID = input[i].ID;
		output.Append(element);
	}
}