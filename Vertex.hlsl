#pragma pack_matrix(row_major)

struct VS_IN
{
	float3 Pos : POSITION;
	float2 Uv : UV; 
	float3 Normal : NORMAL; 
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Uv : UV;
	float3 Normal : NORMAL;
};

//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VS_OUT VS_main(VS_IN input)
{
	//VS_OUT output = (VS_OUT)0;
	VS_OUT output;

	output.Pos = float4(input.Pos, 1);
	output.Uv = input.Uv;
	output.Normal = input.Normal;

	return output;
}