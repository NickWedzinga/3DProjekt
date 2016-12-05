struct PS_IN
{
	float2 UV : UV;
	float3 normal : NORMAL;
	//float4 pos : POSITION;
	float4 svPos : SV_POSITION;
	int ID : ID;
};


float4 main(PS_IN input) : SV_Target0
{
	//return float4(input.pos.x, 0.0f, 0.0f, 1.0f);
	//return float4(psIN.normal, 1.0f);
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}