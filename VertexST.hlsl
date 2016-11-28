#pragma pack_matrix(row_major)

struct VS_IN
{
	float2 UV : UV;
	float3 normal : NORMAL;
	float3 pos : POSITION;
	int ID : ID;
};

struct VS_OUT
{
	float2 UV : UV;
	float3 normal : NORMAL;
	float4 pos : SV_POSITION;
	float ID : ID;
};


VS_OUT VS_main(VS_IN input)
{
	VS_OUT output;

	output.pos = float4(input.pos, 1.0f);
	output.normal = input.normal;
	output.UV = input.UV;
	output.ID = input.ID;

	return output;
}