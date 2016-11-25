Texture2D tex : register(t0);
SamplerState sampAni : register (s0);

struct PS_IN
{
	float2 UV : UV;
	float4 normal : NORMAL;
	float4 pos : SV_POSITION;
	float ID : ID;
};

struct PS_OUT
{
	float4 color : SV_Target0;
	float4 normal : SV_Target1;
	float4 position : SV_Target2;
};

PS_OUT PS_main(PS_IN input)
{
	float4 texColor = tex.Sample(sampAni, input.UV);

	PS_OUT output;
	output.color = float4(texColor.xyz, input.ID);
	output.normal = float4(0.0f, 0.0f, 0.0f, 1.0f);
	output.position = float4(0.0f, 0.0f, 0.0f, 1.0f);
	return output;
}