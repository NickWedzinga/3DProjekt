#pragma pack_matrix(row_major)

struct PixelInputType
{
	float4 pos : SV_POSITION;
	float2 UV : TEXCOORD0;
};

PixelInputType LightVertexShader(uint id : SV_VERTEXID)
{
	PixelInputType output;
	output.UV = float2((id == 0 || id == 1), (id == 1 || id == 3));
	output.pos = float4((id == 0 || id == 1)*2-1, (id == 0 || id == 2)*2-1, 0, 1);
	return output;
}