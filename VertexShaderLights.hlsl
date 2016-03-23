#pragma pack_matrix(row_major)

struct PixelInputType
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCORD0;
};

PixelInputType LightVertexShader(uint id : SV_VERTEXID)
{
	PixelInputType output;

	// Change the position vector to be 4 units for proper matrix calculations.
	//output.pos = float4(input.pos, 1.0f);
	// Calculate the position of the vertex against the world, view, and projection matrices.
	//output.pos = mul(WorldMatrix2, output.pos);
	//output.pos = mul(ViewMatrix, output.pos);
	//output.pos = mul(ProjMatrix, output.pos);
	// Store the texture coordinates for the pixel shader.
	//output.uv = input.uv;
	//return output;

	output.uv = float2((id == 0 || id == 1), (id == 1 || id == 3));
	output.pos = float4((id == 0 || id == 1)*2-1, (id == 0 || id == 2)*2-1, 0, 1);
	return output;

}