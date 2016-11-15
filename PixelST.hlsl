struct PS_IN
{
	float2 UV : UV;
	float4 normal : NORMAL;
	float4 pos : SV_POSITION;
	float4 ID : ID;
};

struct PS_OUT
{
	float4 terrain : SV_Target0;
	float4 color : SV_Target1;
	float4 normal : SV_Target2;
	float4 position : SV_Target3;
};

PS_OUT PS_main(PS_IN input)
{
	PS_OUT output;
	output.terrain = float4(input.UV, 0.0f, 1.0f);
	output.color = float4(0.0f, 0.0f, 0.0f, input.ID.x);
	output.normal = float4(0.0f, 0.0f, 0.0f, 1.0f);
	output.position = float4(0.0f, 0.0f, 0.0f, 1.0f);
	return output;
	/*return float4(input.UV, 0.0f, 1.0f);*/
}