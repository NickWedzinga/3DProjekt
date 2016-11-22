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
void GS_main(point GS_IN input[1], inout TriangleStream< GSOutput > output)
{
	GSOutput element;

	//GS f�r 1 input point ist f�r triangel, kr�ver mycket mindre arbete p� CPU-sidan, kan vara v�rt att prova impl
	//�ndra input type till point eller n�t
	float3 bbToCam = camPos - input[0].pos;
	bbToCam = normalize(bbToCam);
	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 left = cross(bbToCam, up);
	left = normalize(left);
	up = cross(left, bbToCam);
	up = normalize(up);

	float4 pos[6];
	pos[0] = input[0].pos + float4(left, 0.0f) * 0.05 + float4(up, 0.0f) * 0.05; //top left
	pos[1] = input[0].pos - float4(left, 0.0f) * 0.05 - float4(up, 0.0f) * 0.05; //bottom right
	pos[2] = input[0].pos + float4(left, 0.0f) * 0.05 - float4(up, 0.0f) * 0.05; //bottom left
	pos[3] = input[0].pos + float4(left, 0.0f) * 0.05 + float4(up, 0.0f) * 0.05; //top left
	pos[4] = input[0].pos - float4(left, 0.0f) * 0.05 + float4(up, 0.0f) * 0.05; //top right
	pos[5] = input[0].pos - float4(left, 0.0f) * 0.05 - float4(up, 0.0f) * 0.05; //bottom right

	float2 UV[6];
	UV[0] = float2(0, 0); //top left
	UV[1] = float2(1, 1); //bottom right
	UV[2] = float2(0, 1); //bottom left
	UV[3] = float2(0, 0); //top left
	UV[4] = float2(1, 0); //top right
	UV[5] = float2(1, 1); //bottom right

	element.normal = mul(ProjMatrix, mul(ViewMatrix, mul(RotationMatrix, bbToCam)));
	element.ID = input[0].ID;

	for (uint i = 0; i < 6; ++i)
	{
		element.pos = mul(ProjMatrix, mul(ViewMatrix, mul(WorldMatrix, pos[i])));
		element.UV = UV[i];
		output.Append(element);
	}
}