cbuffer ConstantBuffer2 : register(b1)
{
	float4x4 world;
	float4x4 view;
	float4x4 projection;
	float4 color;
	float4 light;
}

cbuffer ConstantBuffer3 : register(b2)
{
	float4x4 boneWorld[512];
}

struct VS_IN {
	float4	pos		: POSITION;
	float4	nor		: NORMAL;
	float4	col		: COLOR0;
	float2	tex		: TEXCOORD0;
	float4	weight	: BLENDWEIGHT; 
	uint4	idx		: BLENDINDICES;
};

struct VS_OUT {
	float4 pos : SV_POSITION;
	float4 col : COLOR0;
	float2 tex : TEXCOORD0;
};

VS_OUT main(VS_IN input) 
{
	VS_OUT output;

	float4 pos;
	float4 normalHead;
	float3 normal;
	float col;
	float4x4 wvp;

	float bw[3] = (float[3])input.weight;
	float4x4 comb = boneWorld[input.idx[0]] * bw[0];
	if(input.idx[1] <= 512){
		comb += boneWorld[input.idx[1]] * bw[1];
	}
	if(input.idx[2] <= 512){
		comb += boneWorld[input.idx[2]] * bw[2];
	}
	if(input.idx[3] <= 512){
		comb += boneWorld[input.idx[3]] * (1.0f - bw[0] - bw[1] - bw[2]);
	}
	pos = mul(comb, float4(input.pos.xyz, 1.0));

	normalHead = mul(float4(input.pos.xyz + input.nor.xyz, 1.0), comb);

	float4x4 w = world;

	w._41 = 0.0f;
	w._42 = 0.0f;
	w._43 = 0.0f;

	normal = mul(normalize(input.nor), w).xyz;
	normal = normalize(normal);

	wvp = mul(world, view);
	wvp = mul(wvp, projection);
	output.pos = mul(pos, wvp);

	col = saturate(-dot(normalize(light.xyz), normal));
	col = (col * 0.4f + 0.6f);
	output.col = float4(col, col, col, 1.0f) * color;

	output.tex = input.tex;

	return output;
}