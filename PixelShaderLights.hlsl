Texture2D terrainTex : register(t0);
Texture2D colorTex : register(t1);
Texture2D normalTex : register(t2);
//Texture2D IDTex : register(t3);
SamplerState SampleTypePoint : register(s0);

cbuffer LightBuffer : register (b0)
{
	float3 lightDirection;
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
	float3 camPosition;
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

	float shiny = 5.0f; //hur stor specular cirkel

	colors = colorTex.Sample(SampleTypePoint, input.UV);
	normals = normalTex.Sample(SampleTypePoint, input.UV);
	terrain = terrainTex.Sample(SampleTypePoint, input.UV);
	

	//Calculate Ambient Light
	float3 LA = KA.xyz; //KA comment because material gives it 0

	//Calculate Diffuse Light
	float angle = dot(normals, normalize(camDirection)); //infallsvinkel
	float3 LD = KD.xyz * saturate(angle); //KD comment because material gives it 0

	//calculate specular intensity
	float3 n = dot(normals, normalize(camDirection)) * normals;
	float3 r = (2 * n) - normalize(camDirection);
	float3 rv = dot(r, camDirection);
	float3 LS = KS.xyz * pow(rv, shiny); //KS comment because material gives it 0

	float3 result = (colors.xyz*LA) + terrain.xyz + (colors.xyz*LD) + (colors.xyz*LS);

	return float4(result, 1.0f);
	//return colors;
}