cbuffer ConstantBuffer : register(b1)
{
	float4x4 world;
	float4x4 view;
	float4x4 projection;
	float4 color;
	float4 light;
}

struct MATERIAL
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float4 emission;
	float shininess;
	float3 dummy;
};

cbuffer MaterialBuffer : register(b2)
{
	MATERIAL	material;
}

struct VS_IN
{
	float4 pos : POSITION0;
	float2 tex : TEXCOORD0;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float4 col : COLOR;
	float2 tex : TEXCOORD0;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output;

	float4x4 wvp;

	wvp = mul(world, view);
	wvp = mul(wvp, projection);

	output.pos = mul(input.pos, wvp);

	output.col = color;

	output.tex = input.tex;

	return output;
}