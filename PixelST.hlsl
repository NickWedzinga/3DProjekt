struct PS_IN
{
	float2 UV : UV;
	float4 normal : NORMAL;
	float4 pos : SV_POSITION;
	float4 ID : IDD;
};

//struct PS_OUT
//{
//	float4 color : SV_Target0;
//	float4 objColor : SV_Target1;
//	float4 normal : SV_Target2;
//	float4 ID : SV_Target3;
//	float4 camRelObj : SV_Target4;
//};

float4 PS_main(PS_IN input) : SV_Target0
{
	//PS_OUT output;
	//output.color = float4(input.UV, 0.0f, 1.0f);
	//output.objColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	//output.normal = float4(0.0f, 0.0f, 0.0f, 1.0f);
	//output.ID = input.ID;
	//output.camRelObj = float4(0.0f, 0.0f, 0.0f, 1.0f);
	//return output;
	return float4(input.UV, 0.0f, 1.0f);
}