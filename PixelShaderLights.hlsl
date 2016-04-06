Texture2D colorTex : register(t0);
Texture2D normalTex : register(t1);
SamplerState SampleTypePoint : register(s0);

cbuffer LightBuffer : register(b0)
{
	float3 lightDirection;
	float padding;	//För att få 16-bytes
};


cbuffer CONSTANT_BUFFER2 : register (b1)
{
	float4 KD;
	float4 KA;
	float4 KS;
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
	float lightIntensity;
	float4 outputColor;

	float3 viewDirection = float3(0.0f, 0.0f, -1.0f);
	float shiny = 5.0f; //hur stor specular cirkel

	colors = colorTex.Sample(SampleTypePoint, input.uv);
	normals = normalTex.Sample(SampleTypePoint, input.uv);

	float3 lightvec = normalize(-lightDirection);	

	//Calculate Ambient Light
	float3 LA = KA.xyz; //KA comment because material gives it 0

	//Calculate Diffuse Light
	float angle = dot(normals, lightvec); //infallsvinkel
	float3 LD = KD.xyz * saturate(angle); //KD comment because material gives it 0

	//calculate specular intensity
	float3 n = dot(normals, lightvec) * normals;
	float3 r = (2 * n) - lightvec;
	float3 rv = dot(r, viewDirection);
	float3 LS = KS.xyz * pow(rv, shiny); //KS comment because material gives it 0

	float3 result = (colors.xyz*LA) + (colors.xyz*LD) + (colors.xyz*LS);

	return float4(result, 1);
}