Texture2D terrainTex : register(t0);
Texture2D colorTex : register(t1); //w holds camera angle compared to normal (specular)
Texture2D normalTex : register(t2);
Texture2D depthTex : register(t3);
//Texture2D IDTex : register(t3);
//Texture2D camRelObj : register(t3);
SamplerState SampleTypePoint : register(s0);

cbuffer LightBuffer : register (b0)
{
	float3 lightPos;
	float padding;	//För att få 16-bytes (float4 (float3 + float = float4))
};


cbuffer CONSTANT_BUFFER2 : register (b1)
{
	float4 KD;
	float4 KA;
	float4 KS;
};

cbuffer CONSTANT_BUFFER : register (b2)
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
	float4 depth;

	colors = colorTex.Sample(SampleTypePoint, input.UV);
	normals = normalTex.Sample(SampleTypePoint, input.UV);
	terrain = terrainTex.Sample(SampleTypePoint, input.UV);
	depth = depthTex.Sample(SampleTypePoint, input.UV);

	float3 lightToPoint = normalize(depth.xyz - lightPos);
	float3 camToPoint = normalize(depth.xyz - camPos);
	float shiny = 0.0005f; //hur stor specular cirkel
	float diffuseAngle = 0;

	diffuseAngle = dot(lightToPoint, -normals.xyz);

	//Calculate Ambient Light
	float3 LA = KA.xyz;

	//Calculate Diffuse Light
	float3 LD = KD.xyz * saturate(diffuseAngle);

	//calculate specular intensity
	float3 Reflection = lightToPoint + 2 * normals.xyz * dot(-normals.xyz, ligthToPoint);
	float RcDir = dot(Reflection, camToPoint);
	float3 LS = KS.xyz * pow(RcDir, shiny);

	float3 result = (colors.xyz*LA) + terrain.xyz + (colors.xyz*LD) + (colors.xyz*LS);

	return float4(result, 1.0f);
	//return float4(LA, 1);
}