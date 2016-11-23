Texture2D colorTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D positionTex : register(t2);
SamplerState SampleTypePoint : register(s0);

RWStructuredBuffer<uint> pickingBuffer : register(u1);

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
};

struct PixelInputType
{
	float4 pos : SV_POSITION;
	float2 UV : TEXCOORD0;
};

float4 LightPixelShader(PixelInputType input) : SV_Target0
{
	float4 colors;
	float4 normals;
	float4 position;

	colors = colorTex.Sample(SampleTypePoint, input.UV);
	normals = normalTex.Sample(SampleTypePoint, input.UV);
	position = positionTex.Sample(SampleTypePoint, input.UV);

	float3 lightToPoint = normalize(position.xyz - lightPos);
	float3 camToPoint = normalize(position.xyz - camPos);
	float shiny = 0.05f;
	float diffuseAngle = 0;

	diffuseAngle = dot(-normals.xyz, lightToPoint);

	//return float4(colors.w / 4.0f, 0, 0, 1); bra debug med deferred

	//Samples the middle pixel of the color texture and sends the w-value (ID) back to the CPU
	pickingBuffer[0] = colorTex.Sample(SampleTypePoint, float2(0.5f, 0.5f)).w + 0.5f;

	//Calculate Ambient Light
	float LA = 1;

	//Calculate Diffuse Light
	float LD = saturate(diffuseAngle);

	//calculate specular intensity
	float3 Reflection = lightToPoint + 2 * normals.xyz * dot(-normals.xyz, lightToPoint);
	float RcDir = saturate(dot(Reflection, /*camToPoint*/camDirection));
	float LS = pow(RcDir, shiny);

	float3 result = (colors.xyz*LA) + (colors.xyz*LD) /*+ (colors.xyz*LS)*/;

	return float4(result, 1.0f);
}