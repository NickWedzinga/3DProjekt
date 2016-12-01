Texture2D colorTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D positionTex : register(t2);
//Texture2D light1Tex : register(t3);
//Texture2D light2Tex : register(t4);
//Texture2D light3Tex : register(t5);
//Texture2D light4Tex : register(t6);
//Texture2D light5Tex : register(t7);
SamplerState SampleTypePoint : register(s0);

RWStructuredBuffer<uint> pickingBuffer : register(u1);

cbuffer CONSTANT_BUFFER : register (b0)
{
	matrix WorldMatrix;
	matrix ViewMatrix;
	matrix ProjMatrix;
	float3 camDirection;
	float3 camPos;
};

cbuffer Lights_Buffer : register (b1)
{
	float3 noLights;
	float3 position;
	float3 intensity;
	float3 color;
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
	float4 light1;
	//float4 light2;
	//float4 light3;
	//float4 light4;
	//float4 light5;
	float3 result;

	normals = normalTex.Sample(SampleTypePoint, input.UV);
	position = positionTex.Sample(SampleTypePoint, input.UV);
	//light1 = light1Tex.Sample(SampleTypePoint, input.UV);
	//light2 = light2Tex.Sample(SampleTypePoint, input.UV);
	//light3 = light3Tex.Sample(SampleTypePoint, input.UV);
	//light4 = light4Tex.Sample(SampleTypePoint, input.UV);
	//light5 = light5Tex.Sample(SampleTypePoint, input.UV);


	colors = colorTex.Sample(SampleTypePoint, input.UV);

	//return light1;
	//Samples the middle pixel of the color texture and sends the w-value (ID) back to the CPU
	pickingBuffer[0] = colorTex.Sample(SampleTypePoint, float2(0.5f, 0.5f)).w + 0.5f;

	if (colors.w < 2.2f && colors.w > 1.8f)	//Only calculating lightning for the cube
	{
		float3 lightToPoint = normalize(position.xyz - float3(15, 0, -3)/*camPos/*lightPos*/);
		//lightToPoint = float3(1.0f, 1.0f, 1.0f);
		float3 camToPoint = normalize(position.xyz - camPos);
		float shiny = 1.0f;
		float diffuseAngle = 0;

		diffuseAngle = dot(-normals.xyz, lightToPoint);

		//return float4(colors.w / 4.0f, 0, 0, 1); bra debug med deferred
		//return float4(position.z, 0, 0, 1);
		//return position;
		
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
	return float4(result, 1.0f);
}