cbuffer CONSTANT_BUFFER : register(b0)
{
	matrix WorldMatrix;
	matrix ViewMatrix;
	matrix ProjMatrix;
	float3 camDirection;
	float3 camPos;
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
	float2 UV : UV;
	float4 normal : NORMAL;
	float4 pos : SV_POSITION;
	float4 worldPos : POSITION;
	float ID : ID;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	float3 camDirection : CAMDIRECTION;
};

float3 CalculatefaceNormal(GS_IN input[3])
{
	float3 p0 = input[0].pos.xyz;
	float3 p1 = input[1].pos.xyz;
	float3 p2 = input[2].pos.xyz;

	float3 v1 = p1 - p0;
	float3 v2 = p2 - p0;

	float3 localNormal = -cross(v2, v1);
	float3 worldNormal = mul(WorldMatrix, localNormal);

	return worldNormal;
}

[maxvertexcount(36)]

//-----------------------------------------------------------------------------------------
// GeometryShader: GSScene
//-----------------------------------------------------------------------------------------
void GS(triangle GS_IN input[3], inout TriangleStream< GS_OUT > Outputstream)
{
	GS_OUT Output;

	float3 worldNormal = CalculatefaceNormal(input);
	float3 vec = normalize(input[0].pos - camPos);
	float angle = dot(vec, worldNormal);

	if (angle <= 0.0f) //Backface Culling
	{

		//Normal mapping ----------------------------------------------------------------
		float3 deltaPos1 = float3(0, 0, 0);
		float3 deltaPos2 = float3(0, 0, 0);
		float2 deltaUV1 = float2(0, 0);
		float2 deltaUV2 = float2(0, 0);
		float3 tangent = float3(0, 0, 0);
		float3 bitangent = float3(0, 0, 0);

		deltaPos1 = input[0].pos.xyz - input[1].pos.xyz; //en vektor i planet
		deltaPos2 = input[0].pos.xyz - input[2].pos.xyz; //en vektor i planet
		deltaUV1 = input[0].UV - input[1].UV; //vektor i texturen
		deltaUV2 = input[0].UV - input[2].UV; //vektor i texturen

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x); //denominator
		tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r; //tangent calculation
		bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r; //bitangent calculation

		tangent = normalize(mul(WorldMatrix, tangent));
		bitangent = normalize(mul(WorldMatrix, bitangent));

		Output.tangent = tangent;
		Output.bitangent = bitangent;

		for (uint i = 0; i < 3; i++)
		{
			float4 poss = input[i].pos;
			poss = mul(WorldMatrix, poss);
			poss = mul(ViewMatrix, poss);
			poss = mul(ProjMatrix, poss);
			Output.pos = poss;
			Output.worldPos = poss;
			Output.camDirection = camDirection;
			Output.UV = input[i].UV;
			Output.normal = mul(WorldMatrix, input[i].normal);
			Output.ID = input[i].ID;

			Outputstream.Append(Output);
		}
	}
	Outputstream.RestartStrip();
}