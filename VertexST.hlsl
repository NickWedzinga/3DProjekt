#pragma pack_matrix(row_major)

struct VS_IN
{
	float2 UV : UV;
	float3 normal : NORMAL;
	float3 pos : POSITION;
};

struct VS_OUT
{
	float2 UV : UV;
	float4 normal : NORMAL;
	float4 pos : SV_POSITION;
};


VS_OUT VS_main(VS_IN input)
{
	VS_OUT output;

	output.pos = float4(input.pos, 1);
	output.normal = float4(input.normal, 0);
	output.UV = input.UV;

	return output;
}