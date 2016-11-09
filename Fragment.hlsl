Texture2D txDiffuse : register(t0);
Texture2D txNormalDiffuse : register(t1);
SamplerState sampAni : register (s0);

cbuffer KEY_BUFFER : register(b0)
{
	float3 normalMap;
};

struct PS_IN
{
	float2 UV : UV;
	float3 Normal : NORMAL;
	float4 Pos : SV_POSITION;
	float4 ID : IDD;
	float4 WorldPos : WORLDPOS;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	float3 camDirection : CAMDIRECTION;
};

struct PS_OUT
{
	float4 terrain : SV_Target0;
	float4 color : SV_Target1;
	float4 normal : SV_Target2;
	//float4 specular : SV_Target3;
	//float4 ID : SV_Target4;	
};

PS_OUT PS_main(PS_IN input)
{
	PS_OUT output;

	float4 newNormal = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 textur = txNormalDiffuse.Sample(sampAni, input.UV);
	float4 color = txDiffuse.Sample(sampAni, input.UV);
	textur = normalize((textur * 2.0f) - 1.0f);
	
	newNormal.xyz = (textur.x * input.tangent) + (textur.y * input.bitangent) + (textur.z * -input.Normal); //z inverterat, dubbelkolla
	newNormal = normalize(newNormal);

	output.terrain = float4(0.0f, 0.0f, 0.0f, 1.0f);
	output.color = color;
	output.color.w = dot(input.Normal, input.camDirection); //w in color holds specular
	if(normalMap.x == 1)
		output.normal = float4(input.Normal, 1.0f);
	else
		output.normal = newNormal;
	//output.ID = input.ID;
	//output.camRelObj = input.camRelObj;

	return output;
};
