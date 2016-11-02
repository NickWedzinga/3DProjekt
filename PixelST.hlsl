struct PS_IN
{
	float4 Pos : SV_POSITION;
	float2 uv : UV;
};

float4 PS_main(PS_IN input) : SV_Target0
{

	return (input.uv, 0, 1);
	//return float4(0.0f, 0.0f, 1.0f, 0.0f);
}