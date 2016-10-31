struct PS_IN
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;
};

float4 PS_main(PS_IN input) : SV_Target0
{

	return input.Color;
	//return float4(0.0f, 0.0f, 1.0f, 0.0f);
}