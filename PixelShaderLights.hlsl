Texture2D terrainTex : register(t0);
Texture2D colorTex : register(t1);
Texture2D normalTex : register(t2);
Texture2D positionTex : register(t3);
//Texture2D IDTex : register(t3);
//Texture2D camRelObj : register(t3);
SamplerState SampleTypePoint : register(s0);

RWStructuredBuffer<uint> pickingBuffer : register(u1);

cbuffer LightBuffer : register (b0)
{
	float3 lightPos;
	float padding;	//För att få 16-bytes (float4 (float3 + float = float4))
};

cbuffer CONSTANT_BUFFER : register (b1)
{
	matrix WorldMatrix;
	matrix ViewMatrix;
	matrix ProjMatrix;
	float3 camDirection;
	float3 camPos;
};



struct PixelInputType
{
	float4 pos : SV_POSITION;
	float2 UV : TEXCORD0;
};

float4 LightPixelShader(PixelInputType input) : SV_Target0
{
	float4 colors;
	float4 normals;
	float4 terrain;
	float4 position;



	colors = colorTex.Sample(SampleTypePoint, input.UV);
	normals = normalTex.Sample(SampleTypePoint, input.UV);
	terrain = terrainTex.Sample(SampleTypePoint, input.UV);
	position = positionTex.Sample(SampleTypePoint, input.UV);

	float3 lightToPoint = normalize(position.xyz - lightPos);
	float3 camToPoint = normalize(position.xyz - camPos);
	float shiny = 0.05f; //hur stor specular cirkel
	float diffuseAngle = 0;

	diffuseAngle = dot(-normals.xyz, lightToPoint);
	

	if (input.UV.x <= 0.51f && input.UV.x >= 0.49f && input.UV.y <= 0.51f && input.UV.y >= 0.49f)	//Does not work with UV==0.5. No pixel has that value
	{
		pickingBuffer[0] = colors.w;
	}

	//Calculate Ambient Light
	float LA = 1/*0.5*/;

	//Calculate Diffuse Light
	float LD = saturate(diffuseAngle);

	//calculate specular intensity
	float3 Reflection = lightToPoint + 2 * normals.xyz * dot(-normals.xyz, lightToPoint);
	float RcDir = saturate(dot(Reflection, camDirection/*camToPoint*/));
	float LS = pow(RcDir, shiny);

	float3 result = (colors.xyz*LA) + (colors.xyz*LD) + (colors.xyz*LS) + terrain.xyz;

	return float4(result, 1.0f);
	//return float4(RcDir, 0.0f, 0.0f, 1.0f);
	//return terrain;
}