Texture2D txDiffuse: register(t0);
SamplerState sampAni;

struct PS_IN
{
	float4 Pos : SV_POSITION;
	float2 Uv : UV; 
	float3 Normal : NORMAL;
	float3 WorldPos : WORLDPOS;
};

cbuffer CONSTANT_BUFFER : register(b0)
{
	matrix WorldMatrix;
	matrix WorldMatrix2;
	matrix WorldMatrix3;
	matrix ViewMatrix;
	matrix ProjMatrix;
};

float4 PS_main(PS_IN input) : SV_Target
{
	float3 lightVec = float3(0.0f, 0.0f, -5.0f);
	float3 camVec = lightVec - input.WorldPos.xyz;
	camVec = normalize(camVec);

	float angle = max(dot(camVec, input.Normal), 0.0f);

	float3 s = txDiffuse.Sample(sampAni, input.Uv).xyz;
	s = s * angle;
	return float4(s, 1.0f);
};