#include "common.hlsli"

struct VS_OUT {
	float4 position : SV_POSITION;
	float4 color : COLOR0;
	float2 texcoord : TEXCOORD0;
};

float4 main(VS_OUT input) : SV_TARGET
{
	clip(myTexture.Sample(mySampler, input.texcoord).w - 1.175494351e-38f);

	return myTexture.Sample(mySampler, input.texcoord) * input.color;
}