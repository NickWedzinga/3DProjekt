cbuffer CONSTANT_BUFFER : register(b0)
{
	matrix WorldMatrix;
	matrix ViewMatrix;
	matrix ProjMatrix;
	float3 camDirection;
};

struct GS_IN
{
	float4 Pos : SV_POSITION;
	float2 uv : UV;
};

struct GSOutput
{
	float4 pos : SV_POSITION;
	float2 uv : UV;
};

[maxvertexcount(48)]
void GS_main(triangle GS_IN input[3], inout TriangleStream< GSOutput > output)
{
	GSOutput element;

	for (uint i = 0; i < 3; i++)
	{
		float4 poss = input[i].Pos;
		poss = mul(WorldMatrix, poss);
		poss = mul(ViewMatrix, poss);
		poss = mul(ProjMatrix, poss);
		element.pos = poss;
		element.uv = input[i].uv;
		output.Append(element);
	}
}