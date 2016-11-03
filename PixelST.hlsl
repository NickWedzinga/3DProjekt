struct PS_IN
{
	float2 UV : UV;
	float4 normal : NORMAL;
	float4 pos : SV_POSITION;
};

float4 PS_main(PS_IN input) : SV_Target0
{
	return (input.UV, 0, 1);
}