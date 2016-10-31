#pragma pack_matrix(row_major)

struct VS_IN
{
	float3 Pos : POSITION;
	float4 Color : COLOR;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;
};


VS_OUT VS_main(VS_IN input)
{
	VS_OUT output;

	output.Pos = float4(input.Pos, 1);
	output.Color = input.Color;

	return output;
}