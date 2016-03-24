Texture2D txDiffuse : register(t0);
SamplerState sampAni : register (s0);

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

	output.color = txDiffuse.Sample(sampAni, input.Uv);
	output.normal = float4(normalize(input.Normal), 1.0f);

	return output;
};
