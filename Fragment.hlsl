Texture2D txDiffuse: register(t0);
SamplerState sampAni;

struct PS_IN
{
	float4 Pos : SV_POSITION;
	float2 Uv : UV; 
	float3 Normal : NORMAL;
	float4 WorldPos : WORLDPOS;
};

cbuffer CONSTANT_BUFFER : register(b0)
{
	matrix WorldMatrix;
	matrix ViewMatrix;
	matrix ProjMatrix;
};

cbuffer CONSTANT_BUFFER2 : register(b1)
{
	float4 KD;
	float4 KA;
	float4 KS;
}

float4 PS_main(PS_IN input) : SV_Target
{
	float3 viewDirection = float3(0.0f, 0.0f, -1.0f);
	float3 lightray = float3(0.0f, 0.0f, -3.0f); //hårdkodat ljus
	float3 amblight = float3(0.5f, 0.5f, 0.5f); //halva färgen kvar 0.5
	float3 difflight = float3(1.0f, 1.0f, 1.0f);
	float3 speclight = float3(1.0f, 1.0f, 1.0f);
	float shiny = 5.0f; //hur stor specular cirkel

	float3 lightvec = lightray - input.WorldPos.xyz;
	lightvec = normalize(lightvec);
	input.Normal = normalize(input.Normal);
	float angle = dot(input.Normal, lightvec); //infallsvinkel
	
	//Calculate Ambient Light
	float3 LA = /*KA * */amblight; //KA comment because material gives it 0

	//Calculate Diffuse Light
	float3 LD = /*KD **/ difflight * saturate(angle); //KD comment because material gives it 0

	//calculate specular intensity
	float3 n = dot(input.Normal, lightvec) * input.Normal;
	float3 r = 2 * n - lightvec;
	float3 rv = dot(r, viewDirection);
	float3 LS = /*KS * */speclight * pow(rv, shiny); //KS comment because material gives it 0

	float3 s = txDiffuse.Sample(sampAni, input.Uv).xyz;
	
	float3 result = s * LA + s * LD + s * LS;
	return float4(result, 1.0f);
};