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
void GS_main(triangle GS_IN input[3], /*point GS_IN input,*/ inout TriangleStream< GSOutput > output)
{
	GSOutput element;

	/*float3 center = (1 / 3) * (input[0].pos + input[1].pos + input[2].pos);
	float3 bbToCam = camPos - center;
	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 left = cross(bbToCam, up);
	up = cross(left, bbToCam);
	float angle = dot(bbToCam, input[0].normal);*/

	//GS för 1 input point ist för triangel, kräver mycket mindre arbete på CPU-sidan, kan vara värt att prova impl
	//ändra input type till point eller nåt
	/*float3 bbToCam = camPos - input.pos;
	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 left = cross(bbToCam, up);
	up = cross(left, bbToCam);
	float4 billboard[6]; 
	billboard[0] = input.pos + float4(left, 0.0f) + float4(up, 0.0f) * 2; //top left
	billboard[1] = input.pos - float4(left, 0.0f) - float4(up, 0.0f) * 2; //bottom right
	billboard[2] = input.pos + float4(left, 0.0f) - float4(up, 0.0f) * 2; //bottom left
	billboard[3] = input.pos + float4(left, 0.0f) + float4(up, 0.0f) * 2; //top left
	billboard[4] = input.pos - float4(left, 0.0f) + float4(up, 0.0f) * 2; //top right
	billboard[5] = input.pos - float4(left, 0.0f) - float4(up, 0.0f) * 2; //bottom right
	element.pos = mul(ProjMatrix, mul(ViewMatrix, mul(RotationMatrix, input.pos)));
	element.normal = mul(ProjMatrix, mul(ViewMatrix, mul(RotationMatrix, input.pos)));
	element.UV = input.UV;
	element.ID = input.ID;
	output.Append(element)
	*/

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