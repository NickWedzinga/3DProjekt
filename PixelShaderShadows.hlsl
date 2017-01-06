struct PS_IN
{
	float2 UV : UV;
	float4 normal : NORMAL;
	float4 pos : POSITION;
	int ID : ID;
};

float4 PS_main(PS_IN input) : SV_Target0
{
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}