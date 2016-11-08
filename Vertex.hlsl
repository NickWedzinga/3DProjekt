#pragma pack_matrix(row_major)

struct VS_IN
{
	float2 UV : UV;
	float3 Normal : NORMAL;
	float3 Pos : POSITION; 
	int ID : IDD;
};

struct VS_OUT
{
	float2 UV : UV;
	float3 Normal : NORMAL;
	float4 Pos : SV_POSITION;
	int ID : IDD;
};

//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VS_OUT VS_main(VS_IN input)
{
	//VS_OUT output = (VS_OUT)0;
	VS_OUT output;

	output.Pos = float4(input.Pos, 1.0f);
	output.UV = input.UV;
	output.Normal = input.Normal;
	output.ID = float4(input.ID, 0.0f, 0.0f, 0.0f);

	return output;
}