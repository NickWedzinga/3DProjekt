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

struct GSOutput
{
	float2 UV : UV;
	float4 normal : NORMAL;
	float4 pos : SV_POSITION;
	float4 worldPos : WORLDPOS;
	float ID : ID;
};


[maxvertexcount(6)]
void GS_main(point GS_IN input[1], inout TriangleStream< GSOutput > output)
{
	GSOutput element;

	float3 camToBB = input[0].pos - camPos;
	camToBB = normalize(camToBB);
	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 left = cross(camToBB, up); //order of cross product gives left rather than right vec
	left = normalize(left);
	up = cross(left, camToBB); //-left funkar inte för up blir uppochner
	up = normalize(up);
	float scaleFactor = 0.5;
	if (input[0].ID == 7)
		scaleFactor = 0.1f;
	float4 pos[4];

	pos[0] = input[0].pos - float4(left, 0.0f) * scaleFactor + float4(up, 0.0f) * scaleFactor; //top right
	pos[1] = input[0].pos - float4(left, 0.0f) * scaleFactor - float4(up, 0.0f) * scaleFactor; //bottom right
	pos[2] = input[0].pos + float4(left, 0.0f) * scaleFactor + float4(up, 0.0f) * scaleFactor; //top left
	pos[3] = input[0].pos + float4(left, 0.0f) * scaleFactor - float4(up, 0.0f) * scaleFactor; //bottom left

	float2 UV[4];

	UV[0] = float2(1, 0); //top right
	UV[1] = float2(1, 1); //bottom right
	UV[2] = float2(0, 0); //top left
	UV[3] = float2(0, 1); //bottom left

	element.normal = mul(ProjMatrix, mul(ViewMatrix, mul(WorldMatrix, -camToBB)));
	element.ID = input[0].ID;

	for (uint i = 0; i < 4; ++i)
	{
		pos[i].w = 1.0f;
		float4 poss = pos[i];
		poss = mul(WorldMatrix, poss);
		element.worldPos = poss;
		poss = mul(ViewMatrix, poss);
		poss = mul(ProjMatrix, poss);
		element.pos = poss;
		element.UV = UV[i];
		output.Append(element);
	}
}