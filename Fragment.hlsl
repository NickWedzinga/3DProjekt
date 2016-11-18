Texture2D txDiffuse : register(t0);
Texture2D txNormalDiffuse : register(t1);
SamplerState sampAni : register (s0);

cbuffer KEY_BUFFER : register(b0)
{
	float3 normalMap;
};

cbuffer CONSTANT_BUFFER2 : register (b1)
{
	float4 KD;
	float4 KA;
	float4 KS;
};

struct PS_IN
{
	float2 UV : UV;
	float3 normal : NORMAL;
	float4 pos : SV_POSITION; //implicit use by pixel shaders, has to be world coords, even though it is 
	float4 worldPos : POSITION;
	float4 ID : ID;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	float3 camDirection : CAMDIRECTION;
};

struct PS_OUT
{
	float4 color : SV_Target0;
	float4 normal : SV_Target1;
	float4 position : SV_Target2;
};

PS_OUT PS_main(PS_IN input)
{
	PS_OUT output;

	float4 newNormal = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 textur = txNormalDiffuse.Sample(sampAni, input.UV);
	float4 color = txDiffuse.Sample(sampAni, input.UV);
	textur = normalize((textur * 2.0f) - 1.0f);

	output.color = color * KA + color * KD + color * KS;
	output.color.w = input.ID.x;
	output.position = input.worldPos;

	if(normalMap.x == 1)
		output.normal = float4(input.normal, 1.0f);
	else
	{
		newNormal.xyz = (textur.x * input.tangent) + (textur.y * input.bitangent) + (textur.z * -input.normal); //z inverterat, dubbelkolla
		newNormal = normalize(newNormal);
		output.normal = newNormal;
	}
	return output;
};
