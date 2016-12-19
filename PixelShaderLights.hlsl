#define epsilon 0.005f	//0.00005f works
/**
When shadowRes == 1, epsilon should be >= 0.00003f
When shadowRes == 10 epsilon should be >= 0.000003f
*/

Texture2D colorTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D positionTex : register(t2);
Texture2D light1Tex : register(t3);
//Texture2D light2Tex : register(t4);
//Texture2D light3Tex : register(t5);
//Texture2D light4Tex : register(t6);
//Texture2D light5Tex : register(t7);

SamplerState SampleTypePoint : register (s0);

RWStructuredBuffer<uint> pickingBuffer : register(u1);

cbuffer CONSTANT_BUFFER : register (b0)
{
	matrix WorldMatrix;
	matrix ViewMatrix;
	matrix ProjMatrix;
	float3 camDirection;
	float3 camPos;
};

struct lightStruct
{
	matrix View;
	matrix Proj;
	float3 noLights;
	float3 lightPosition;
	float3 intensity;
	float3 color;
	float3 direction;
	float3 lightDistance;
};

cbuffer lights : register (b1)
{
	lightStruct lights[1];
	/*matrix View;
	matrix Proj;
	float3 noLights;
	float3 lightPosition;
	float3 intensity;
	float3 color;
	float3 direction;
	float3 lightDistance;*/
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
	//float4 light1;
	//float4 light2;
	//float4 light3;
	//float4 light4;
	//float4 light5;
	float3 result;
	float shadowRes = 1;

	normals = normalTex.Sample(SampleTypePoint, input.UV);
	colors = colorTex.Sample(SampleTypePoint, input.UV);
	//Samples the middle pixel of the color texture and sends the w-value (ID) back to the CPU
	pickingBuffer[0] = colorTex.Sample(SampleTypePoint, float2(0.5f, 0.5f)).w + 0.5f;
	//light2 = light2Tex.Sample(SampleTypePoint, input.UV);
	//light3 = light3Tex.Sample(SampleTypePoint, input.UV);
	//light4 = light4Tex.Sample(SampleTypePoint, input.UV);
	//light5 = light5Tex.Sample(SampleTypePoint, input.UV);
	int i = 0;
	//for (uint i = 0; i < lights[0].noLights.x; ++i)
	//{
	position = positionTex.Sample(SampleTypePoint, input.UV);

	position.w = 1.0f;
	position = mul(lights[i].View, position);
	position = mul(lights[i].Proj, position);
	position /= position.w; //convert to NDC

	float2 shadowmapCoord = float2(position.x * 0.5, position.y * -0.5f) + float2(0.5f, 0.5f); //from -1, 1 to 0, 1 DUBBELKOLLA

	float dx = 1.0f / (960.0f*shadowRes); //divide by SMAP_WIDTH
	float dy = 1.0f / (540.0f*shadowRes); //divide by SMAP_HEIGHT

	float s0 = (light1Tex.Sample(SampleTypePoint, shadowmapCoord).x + epsilon < position.z) ? 0.0f : 1.0f; //DUBBELKOLLA, .x istället för .r?
	float s1 = (light1Tex.Sample(SampleTypePoint, shadowmapCoord + float2(dx, 0.0f)).x + epsilon < position.z) ? 0.0f : 1.0f;
	float s2 = (light1Tex.Sample(SampleTypePoint, shadowmapCoord + float2(0.0f, dy)).x + epsilon < position.z) ? 0.0f : 1.0f;
	float s3 = (light1Tex.Sample(SampleTypePoint, shadowmapCoord + float2(dx, dy)).x + epsilon < position.z) ? 0.0f : 1.0f;

	float2 texelPos = float2(shadowmapCoord.x * (960.0f*shadowRes), shadowmapCoord.y * (540.0f*shadowRes)); //.x, .y?

	float2 lerps = frac(texelPos);

	float shadowCoeff = lerp(lerp(s0, s1, lerps.x), lerp(s2, s3, lerps.x), lerps.y); //WHAT


	if (colors.w < 4.2f) //no light calculations for terrain and billboarded particles
	{
		colors.xyz = colors.xyz * shadowCoeff;
		position = positionTex.Sample(SampleTypePoint, input.UV);
		float3 lightPos = lights[i].lightPosition;

		float3 lightToPoint = normalize(lights[i].direction)/*normalize(position.xyz - lightPos)*/;
		float3 camToPoint = normalize(position.xyz - camPos);
		float shiny = 10.0f;
		float diffuseAngle = 0;

		diffuseAngle = dot(-normals.xyz, /*lightToPoint*//*normalize(position.xyz - lightPos)*/normalize(lights[i].direction));

		//Calculate Ambient Light
		float LA = 0.2f;

		//Calculate Diffuse Light
		float LD = saturate(diffuseAngle);

		//Calculate specular intensity
		float4 r = float4(reflect(-lightToPoint, normals), 0.0f);
		float LS = pow(saturate(dot(r, camToPoint)), shiny);

		result = ((colors.xyz * LA) + (colors.xyz * LD) + (colors.xyz * LS)) * lights[i].intensity.x;
	}
	else
	{
		result = colors.xyz;
	}
	//}
	return float4(result, 1.0f);
}



















































//float3 Reflection = lightToPoint + 2 * normals.xyz * dot(-normals.xyz, lightToPoint);
//float RcDir = (dot(Reflection, camToPoint));
//float LS = pow(RcDir, shiny);
