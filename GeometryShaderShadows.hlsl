struct GS_OUT
{
	float2 UV : UV;
	float3 normal : NORMAL;
	float4 svPos : SV_POSITION;
	int ID : ID;
};

struct GS_IN
{
	float2 UV : UV;
	float3 normal : NORMAL;
	float4 svPos : SV_POSITION;
	int ID : ID;
};

[maxvertexcount(46)]
void main(triangle GS_IN input[3],	inout TriangleStream< GS_OUT > output)
{
	for (uint i = 0; i < 3; i++)
	{
		GS_OUT element;
		element.svPos = input[i].svPos;
		element.ID = input[i].ID;
		element.normal = input[i].normal;
		element.UV = input[i].UV;
		output.Append(element);
	}
}