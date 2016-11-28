cbuffer lights : b0
{
	float3 noLights;
	float3 position[5];
	float3 intensity[5];
	float3 color[5];
	float3 direction[5];
	matrix View[5];
	matrix Proj[5];
};

struct GS_IN
{
	float2 UV : UV;
	float3 normal : NORMAL;
	float4 pos : SV_POSITION;
	float ID : ID;
};

struct GS_OUT
{
	float4 pos : SV_POSITION;
	float3 light : LIGHT;
	/*float3 light[5] : LIGHT;*/
};

[maxvertexcount(36)]
void GS_main(triangle GS_IN input[3], inout TriangleStream< GS_OUT > output)
{
	GS_OUT element;
	for (uint i = 0; i < 3; i++)
	{
		for (uint j = 0; j < noLights.x; j++)
		{
			float4 tempPos = input[i].pos;
			tempPos = mul(View[j], tempPos);
			tempPos = mul(Proj[j], tempPos);
			element.light = tempPos.xyz;
			element.pos = tempPos;

			output.Append(element);
		}
	}
}