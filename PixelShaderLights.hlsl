Texture2D terrainTex : register(t0);
Texture2D colorTex : register(t1);
Texture2D normalTex : register(t2);
Texture2D positionTex : register(t3);
//Texture2D IDTex : register(t3);
//Texture2D camRelObj : register(t3);
SamplerState SampleTypePoint : register(s0);

cbuffer LightBuffer : register (b0)
{
	float3 lightPos;
	float padding;	//F�r att f� 16-bytes (float4 (float3 + float = float4))
};

cbuffer CONSTANT_BUFFER : register (b1)
{
	matrix WorldMatrix;
	matrix ViewMatrix;
	matrix ProjMatrix;
	float3 camDirection;
	float3 camPos;
}

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
	float shiny = 0.5f; //hur stor specular cirkel
	float diffuseAngle = 0;

	diffuseAngle = dot(lightToPoint, -normals.xyz);

	//Calculate Ambient Light
	float LA = 0.5f;

	//Calculate Diffuse Light
	float LD = saturate(diffuseAngle);

	//calculate specular intensity
	float3 Reflection = lightToPoint + 2 * normals.xyz * dot(-normals.xyz, lightToPoint);
	float RcDir = dot(Reflection, camToPoint);
	float LS = pow(RcDir, shiny);

	float3 result = (colors.xyz*LA) + terrain.xyz + (colors.xyz*LD) + (colors.xyz*LS);

	return float4(result, 1.0f);
	//return float4(LD, 0.0f, 0.0f, 1.0f);
	//return terrain;
}