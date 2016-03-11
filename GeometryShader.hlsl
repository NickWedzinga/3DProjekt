struct GS_IN
{
	float4 Pos : SV_POSITION;
	float2 Uv : UV;
	float3 Normal : NORMAL;
};

struct GS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Uv : UV; 
	float3 Normal : NORMAL;
	float4 WorldPos : WORLDPOS;
};

cbuffer CONSTANT_BUFFER : register(b0)
{
	matrix WorldMatrix;
	matrix ViewMatrix;
	matrix ProjMatrix;
};

[maxvertexcount(36)]

//-----------------------------------------------------------------------------------------
// GeometryShader: GSScene
//-----------------------------------------------------------------------------------------
void GS(triangle GS_IN input[3], inout TriangleStream< GS_OUT > Outputstream)
{
	GS_OUT Output;

	for (uint i = 0; i < 3; i++)
	{
		float4 poss = input[i].Pos;
		poss = mul(WorldMatrix, poss);
		Output.WorldPos = poss;
		poss = mul(ViewMatrix, poss);
		poss = mul(ProjMatrix, poss);
		Output.Pos = poss;
		Output.Uv = input[i].Uv;
		Output.Normal = mul(WorldMatrix, input[i].Normal);
		Outputstream.Append(Output);
	}	
	Outputstream.RestartStrip();
}