struct PS_IN
{
	float2 UV : UV;
	float4 normal : NORMAL;
	float4 pos : SV_POSITION;
	float4 ID : IDD;
};

struct PS_OUT
{
	float4 color : SV_Target0;
	float4 objColor : SV_Target1;
	float4 normal : SV_Target2;
	float4 ID : SV_Target3;
};

PS_OUT PS_main(PS_IN input) : SV_Target
{
	PS_OUT output;

	output.color = float4(input.UV, 0, 1);
	output.objColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	output.normal = float4(0.0f, 0.0f, 0.0f, 0.0f);
	output.ID = input.ID;
	return output;
}