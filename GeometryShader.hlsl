cbuffer CONSTANT_BUFFER : register(b0)
{
	matrix WorldMatrix;
	matrix ViewMatrix;
	matrix ProjMatrix;
	float3 camDirection;
	float3 camPosition;
};

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

float3 CalculatefaceNormal(GS_IN input[3])
{
	float3 p0 = input[0].Pos;
	float3 p1 = input[1].Pos;
	float3 p2 = input[2].Pos;

	float3 v1 = p1 - p0;
	float3 v2 = p2 - p0;

	float3 localnormal = -cross(v2, v1);
	float3 worldnormal = mul(WorldMatrix, localnormal);

	return worldnormal;
}

[maxvertexcount(48)]

//-----------------------------------------------------------------------------------------
// GeometryShader: GSScene
//-----------------------------------------------------------------------------------------
void GS_main(triangle GS_IN input[3], inout TriangleStream< GS_OUT > Outputstream)
{
	GS_OUT Output;

	float3 worldnormal = CalculatefaceNormal(input);
	float3 vec = normalize(input[0].Pos - camPosition);
	float angle = dot(vec, worldnormal);

	if (angle <= 0.0f)
	{
		for (uint i = 0; i < 3; i++)
		{
			float4 poss = input[i].Pos;
			poss = mul(WorldMatrix, poss);
			poss = mul(ViewMatrix, poss);
			poss = mul(ProjMatrix, poss);
			Output.Pos = poss;
			Output.WorldPos = poss;
			Output.Uv = input[i].Uv;
			Output.Normal = mul(WorldMatrix, input[i].Normal);
			Outputstream.Append(Output);
		}
	}
	Outputstream.RestartStrip();
}