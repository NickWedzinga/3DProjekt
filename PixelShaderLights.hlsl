Texture2D colorTex : register(t0);
Texture2D normalTex : register(t1);
SamplerState SampleTypePoint : register(s0);

cbuffer LightBuffer : register(b0)
{
	float3 lightDirection;
	float padding;	//För att få 16-bytes
};

struct PixelInputType
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCORD0;
};

float4 LightPixelShader(PixelInputType input) : SV_Target0
{
	float4 colors;
	float4 normals;
	float3 lightDir;
	float lightIntensity;
	float4 outputColor;

	colors = colorTex.Sample(SampleTypePoint, input.uv);
	normals = normalTex.Sample(SampleTypePoint, input.uv);

	lightDir = -lightDirection;
	lightIntensity = saturate(dot(normals.xyz, lightDir));
	outputColor = saturate(colors*lightIntensity);

	return (outputColor);
}