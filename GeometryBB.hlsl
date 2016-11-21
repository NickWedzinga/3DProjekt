cbuffer CONSTANT_BUFFER : register(b0)
{
	matrix WorldMatrix;
	matrix ViewMatrix;
	matrix ProjMatrix;
	float3 camDirection;
	float3 camPos;
};

cbuffer rotation : register(b1)
{
	matrix RotationMatrix;
}

struct GS_IN
{
	float2 UV : UV;
	float4 normal : NORMAL;
	float4 pos : SV_POSITION;
	float4 ID : ID;
};

struct GSOutput
{
	float2 UV : UV;
	float4 normal : NORMAL;
	float4 pos : SV_POSITION;
	float4 ID : ID;
};

[maxvertexcount(6)]
void GS_main(triangle GS_IN input[3], inout TriangleStream< GSOutput > output)
{
	GSOutput element;

	float3 center = (1 / 3) * (input[0].pos + input[1].pos + input[2].pos);
	float3 bbToCam = camPos - center;
	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 left = cross(bbToCam, up);
	up = cross(left, bbToCam);

	/*float4 posNew[3];
	posNew[0]= input[0].pos + left * 2 + up * 2;
	posNew[1] = input[1].pos + left * 2 + up * 2;
	posNew[2] = input[2].pos + left * 2 + up * 2;*/

	
	//float angle = dot(bbToCam, input[0].normal);

	



	for (uint i = 0; i < 3; i++)
	{
		float4 poss = input[i].pos;


		
		//poss = mul(WorldMatrix, poss);
		poss = mul(RotationMatrix, poss);
		poss = mul(ViewMatrix, poss);
		poss = mul(ProjMatrix, poss);
		element.pos = poss;

		float4 normal = input[i].normal;
		normal = mul(WorldMatrix, normal);
		normal = mul(ViewMatrix, normal);
		normal = mul(ProjMatrix, normal);
		element.normal = normal;
		element.UV = input[i].UV;
		element.ID = input[i].ID;
		output.Append(element);
	}
}