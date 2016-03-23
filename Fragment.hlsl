Texture2D txDiffuse : register(t0);
SamplerState sampAni : register (s0);

cbuffer CONSTANT_BUFFER2 : register (b0)
{
	float4 KD;
	float4 KA;
	float4 KS;
};

struct PS_IN
{
	float4 Pos : SV_POSITION;
	float2 Uv : UV; 
	float3 Normal : NORMAL;
	float3 WorldPos : WORLDPOS;
};

struct PS_OUT
{
	float4 color : SV_Target0;
	float4 normal : SV_Target1;
};

PS_OUT PS_main(PS_IN input) : SV_Target
{
	PS_OUT output;

	float3 viewDirection = float3(0.0f, 0.0f, -1.0f);
	float3 lightray = float3(0.0f, 0.0f, -3.0f); //hårdkodat ljus
	float3 amblight = float3(0.5f, 0.5f, 0.5f); //halva färgen kvar 0.5
	float3 difflight = float3(1.0f, 1.0f, 1.0f);
	float3 speclight = float3(0.5f, 0.5f, 0.5f);
	float shiny = 5.0f; //hur stor specular cirkel

	lightray = normalize(lightray);
	float3 lightvec = lightray - input.WorldPos.xyz;
	//input.Normal = normalize(input.Normal);
	float angle = dot(input.Normal, lightvec); //infallsvinkel

	//Calculate Ambient Light
	float3 LA = amblight/*KA*/; //KA comment because material gives it 0

	//Calculate Diffuse Light
	float3 LD = /*KD*/ difflight * saturate(angle); //KD comment because material gives it 0

	//calculate specular intensity
	float3 n = dot(input.Normal, lightvec) * input.Normal;
	float3 r = 2 * n - lightvec;
	float3 rv = dot(r, viewDirection);
	float3 LS = /*KS*/ speclight * pow(rv, shiny); //KS comment because material gives it 0

	float3 s = txDiffuse.Sample(sampAni, input.Uv).xyz;

	float3 result = s * LA + s * LD + s * LS;

	output.color = float4(result, 1.0f);
	output.normal = float4(input.Normal, 1.0f);

	return output;
};
