struct PS_IN
{
	float4 pos : SV_POSITION;
	float3 light : LIGHT;
	/*float3 light[5] : LIGHT;*/
};

struct PS_OUT
{
	float4 light1 : SV_Target0;
	float4 light2 : SV_Target1;
	float4 light3 : SV_Target2;
	float4 light4 : SV_Target3;
	float4 light5 : SV_Target4;
};

float4 PS_main(PS_IN input) : SV_Target0
{
	PS_OUT output;
	//output.light1 = float4(input.light[0], 1.0f);
	//output.light2 = float4(input.light[1], 1.0f);
	//output.light3 = float4(input.light[2], 1.0f);
	//output.light4 = float4(input.light[3], 1.0f);
	//output.light5 = float4(input.light[4], 1.0f);

	return float4(1.0f, 0.0f, 0.0f, 1.0f);;
}