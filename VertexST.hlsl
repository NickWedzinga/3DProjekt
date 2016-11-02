#pragma pack_matrix(row_major)

struct VS_IN
{
	float3 Pos : POSITION;
	float2 uv : UV;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 uv : UV;
};


VS_OUT VS_main(VS_IN input)
{
	VS_OUT output;

	output.Pos = float4(input.Pos, 1);
	output.uv = input.uv;

	return output;
}