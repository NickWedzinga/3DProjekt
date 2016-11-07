#pragma pack_matrix(row_major)

struct VS_IN
{
	float3 Pos : POSITION;
	float2 UV : UV;
	float3 Normal : NORMAL; 
	int ID : IDD;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 UV : UV;
	float3 Normal : NORMAL;
	float4 ID : IDD;
};

//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VS_OUT VS_main(VS_IN input)
{
	//VS_OUT output = (VS_OUT)0;
	VS_OUT output;

	output.Pos = float4(input.Pos, 1);
	output.UV = input.UV;
	output.Normal = input.Normal;
	output.ID = float4(input.ID, 0, 0, 0);

	return output;
}