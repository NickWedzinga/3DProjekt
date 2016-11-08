Texture2D terrainTex : register(t0);
Texture2D colorTex : register(t1);
Texture2D normalTex : register(t2);
//Texture2D IDTex : register(t3);
Texture2D camRelObj : register(t3);
SamplerState SampleTypePoint : register(s0);

cbuffer LightBuffer : register (b0)
{
	float3 lightDirection;
	float padding;	//För att få 16-bytes (float4 (float3 + float = float4))
};


cbuffer CONSTANT_BUFFER2 : register (b1)
{
	float4 KD;
	float4 KA;
	float4 KS;
};

cbuffer CONSTANT_BUFFER : register (b2)
{
	matrix WorldMatrix;
	matrix ViewMatrix;
	matrix ProjMatrix;
	float3 camDirection;
	float3 camLightPos;
}

struct PixelInputType
{
	float4 pos : SV_POSITION;
	float2 UV : TEXCORD0;
};

float4 LightPixelShader(PixelInputType input) : SV_Target0
{
	float4 colors;
	float4 normals;
	float4 terrain;
	float4 camRelObjLoc;

	colors = colorTex.Sample(SampleTypePoint, input.UV);
	normals = normalTex.Sample(SampleTypePoint, input.UV);
	terrain = terrainTex.Sample(SampleTypePoint, input.UV);
	camRelObjLoc = camRelObj.Sample(SampleTypePoint, input.UV);
	camRelObjLoc = normalize(camRelObjLoc);

	float shiny = 1.0f; //hur stor specular cirkel
	float angle = 0;
	float intensity = dot(camRelObjLoc.xyz, camDirection);
	intensity = saturate(intensity);




	if (intensity >= 0) //looking in the direction of the object
	{
		angle = dot(-normals, camRelObjLoc); //infallsvinkel
		//colors = intensity * colors;

		//Calculate Ambient Light
		float3 LA = KA.xyz;

		//Calculate Diffuse Light
		float3 LD = KD.xyz /** intensity*/ * saturate(angle);

		//calculate specular intensity
		float3 V = 2 * normals * dot(-normals, camRelObjLoc);
		float3 R = camRelObjLoc + V;
		float RV = saturate(dot(R, -camDirection));
		/*float3 n = dot(normals, normalize(camDirection)) * normals;
		float3 r = (2 * n) - normalize(camDirection);
		float3 rv = dot(r, camDirection);*/
		float3 LS = KS.xyz * pow(RV, shiny);

		float3 result = (colors.xyz*LA) + terrain.xyz + (colors.xyz*LD) + (colors.xyz*LS);

		return float4(result, 1.0f);
		//return float4(LD, 0, 0, 1);
		//return float4(intensity, 0.0f, 0.0f, 1.0f);
	}
	else //looking away from object
		return float4(0.0f, 0.0f, 0.0f, 1.0f);
}