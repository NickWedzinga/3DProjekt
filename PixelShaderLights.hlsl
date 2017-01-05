#define epsilon 0.0025f	//0.00005f works
/**
When shadowRes == 1, epsilon should be >= 0.00003f
When shadowRes == 10 epsilon should be >= 0.000003f
*/

Texture2D colorTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D positionTex : register(t2);
Texture2D light1Tex : register(t3);

SamplerState SampleTypePoint : register (s0);

RWStructuredBuffer<uint> pickingBuffer : register(u1); //because backbuffer is in u0.

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
	float3 lightPosition;
	float3 intensity;
	float3 direction;
	matrix View;
	matrix Proj;
};

cbuffer lights : register (b1)
{
	/*lightStruct lights[1];*/
	float3 lightPosition;
	float3 intensity;
	float3 direction;
	matrix View;
	matrix Proj;
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
	float3 result;
	float shadowRes = 100;

	normals = normalTex.Sample(SampleTypePoint, input.UV);
	colors = colorTex.Sample(SampleTypePoint, input.UV);
	pickingBuffer[0] = colorTex.Sample(SampleTypePoint, float2(0.5f, 0.5f)).w + 0.5f;
	int i = 0;
	position = positionTex.Sample(SampleTypePoint, input.UV);

	position.w = 1.0f;
	position = mul(View, position);
	position = mul(Proj, position);
	position /= position.w; //convert to NDC

	float2 shadowmapCoord = float2(position.x * 0.5, position.y * -0.5f) + float2(0.5f, 0.5f); //from -1, 1 to 0, 1 DUBBELKOLLA

	float dx = 1.0f / (960.0f*shadowRes); //divide by SMAP_WIDTH
	float dy = 1.0f / (540.0f*shadowRes); //divide by SMAP_HEIGHT

	float s0 = (light1Tex.Sample(SampleTypePoint, shadowmapCoord).x + epsilon < position.z) ? 0.0f : 1.0f; //DUBBELKOLLA, .x istället för .r?
	float s1 = (light1Tex.Sample(SampleTypePoint, shadowmapCoord + float2(dx, 0.0f)).x + epsilon < position.z) ? 0.0f : 1.0f;
	float s2 = (light1Tex.Sample(SampleTypePoint, shadowmapCoord + float2(0.0f, dy)).x + epsilon < position.z) ? 0.0f : 1.0f;
	float s3 = (light1Tex.Sample(SampleTypePoint, shadowmapCoord + float2(dx, dy)).x + epsilon < position.z) ? 0.0f : 1.0f;

	float2 texelPos = float2(shadowmapCoord.x * (960.0f*shadowRes), shadowmapCoord.y * (540.0f*shadowRes)); //.x, .y

	float2 lerps = frac(texelPos);

	float shadowCoeff = lerp(lerp(s0, s1, lerps.x), lerp(s2, s3, lerps.x), lerps.y); //WHAT

	if (colors.w < 4.5f) //no light calculations for terrain and billboarded particles
	{
		colors.xyz = colors.xyz * shadowCoeff;
		position = positionTex.Sample(SampleTypePoint, input.UV);
		float3 lightPos = lightPosition;

		float3 lightToPoint = normalize(direction);
		float4 camToPoint = normalize(position - float4(camPos, 1.0f));
		
		//Calculate Ambient Light
		float LA = 0.2f;

		//Calculate Diffuse Light
		float diffuseAngle = 0;
		diffuseAngle = dot(-normals.xyz, lightToPoint);
		float LD = saturate(diffuseAngle);

		//Calculate specular intensity
		float LS;
		float shiny = 1.0f;
		if (LD >= 0.01f)  //if there is no diffuse, there is no specular
		{
			float4 r = float4(normalize(reflect(lightToPoint, normals)), 0.0f);
			LS = pow(saturate(dot(r, -camToPoint)), shiny);
		}
		else
		{
			LS = 0;
		}

		result = ((colors.xyz * LA) + (colors.xyz * LD) + (colors.xyz * LS)) * intensity.x;
		//return float4(colors.xyz * LA, 0);
	}
	else
	{
		result = colors.xyz;
	}
	return float4(result, 1.0f);
}



















































//float3 Reflection = lightToPoint + 2 * normals.xyz * dot(-normals.xyz, lightToPoint);
//float RcDir = (dot(Reflection, camToPoint));
//float LS = pow(RcDir, shiny);
