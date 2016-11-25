cbuffer lights : (b0)
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
	float4 normal : NORMAL;
	float4 pos : SV_POSITION;
	float ID : ID;
};

struct GS_OUT
{
	float4 pos : SV_POSITION;
	float4 light[5] : LIGHT;
};

[maxvertexcount(36)]
void GS_main(triangle GS_IN input[3], inout TriangleStream< GS_OUT > Outputstream)
{
	GS_OUT element;
	/*float3 points[noLights];*/
	//for (uint i = 0; i < 5; i++)
	//{
	//	element.light[i] = float4(0.0, 0.0, 0.0, 0.0);
	//}
	for (uint i = 0; i < 3; i++)
	{
		for (uint j = 0; j < noLights; j++)
		{
			element.pos = input[i].pos;

			float4 tempPos = input[i].pos;
			tempPos = mul(View[j], tempPos);
			element.light[j] = mul(Proj[j], tempPos);

			output.Append(element);
		}
	}
}