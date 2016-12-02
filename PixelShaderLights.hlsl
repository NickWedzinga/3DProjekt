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
	float4 light1;
	//float4 light2;
	//float4 light3;
	//float4 light4;
	//float4 light5;
	float3 result;

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
		position /= position.w;

		float2 shadowmapCoord = position.xy *= 0.5 + float2(0.5f, 0.5f);
		light1 = light1Tex.Sample(SampleTypePoint, shadowmapCoord);

		if (position.z <= light1.x) //TRUE => lit point
		{
			float3 lightToPoint = normalize(position.xyz - lights[i].lightPosition);
			float3 camToPoint = normalize(position.xyz - camPos);
			float shiny = 1.0f;
			float diffuseAngle = 0;

			diffuseAngle = dot(-normals.xyz, lightToPoint);

			//Calculate Ambient Light
			float LA = 0.7f;

			//Calculate Diffuse Light
			float LD = saturate(diffuseAngle);

			//calculate specular intensity
			float3 Reflection = lightToPoint + 2 * normals.xyz * dot(-normals.xyz, lightToPoint);
			float RcDir = (dot(Reflection, camToPoint));
			float LS = pow(RcDir, shiny);

			result = (colors.xyz * LA) + (colors.xyz*LD) + (colors.xyz*LS);
		}
		else
		{
			result = colors;
		}
	//}
	return float4(result, 1.0f);
}