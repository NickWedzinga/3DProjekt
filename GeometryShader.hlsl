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
	matrix WorldMatrix2;
	matrix WorldMatrix3;
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
	
	//Beräkna Normal, SKA BORT SEN
	/*float3 vek1 = input[0].Pos.xyz - input[1].Pos.xyz;
	float3 vek2 = input[0].Pos.xyz - input[2].Pos.xyz;
	float3 Normal = normalize(cross(vek1, vek2));
	Output.Normal = normalize(mul(WorldMatrix, Normal)); */

	for (uint i = 0; i < 3; i++)
	{
		float4 poss = input[i].Pos;
		poss = mul(WorldMatrix, poss);
		poss = mul(WorldMatrix2, poss); 
		poss = mul(WorldMatrix3, poss);
		poss = mul(ViewMatrix, poss);
		poss = mul(ProjMatrix, poss);
		Output.Pos = poss;
		Output.WorldPos = poss;
		Output.Uv = input[i].Uv;
		Output.Normal = mul(WorldMatrix, input[i].Normal);
		Outputstream.Append(Output);
	}	
	Outputstream.RestartStrip();

	/*for (uint i = 0; i < 3; i++) //QUAD2 SKA BORT SEN
	{
		float4 poss2 = input[i].Pos + float4(Normal * 1.0, 0);
		poss2 = mul(WorldMatrix, poss2);
		poss2 = mul(WorldMatrix2, poss2); 
		poss2 = mul(WorldMatrix3, poss2);
		poss2 = mul(ViewMatrix, poss2);
		poss2 = mul(ProjMatrix, poss2);
		Output.Pos = poss2;
		Output.WorldPos = poss2;
		Output.Uv = input[i].Uv;
		Outputstream.Append(Output);
	}
	Outputstream.RestartStrip();*/
}