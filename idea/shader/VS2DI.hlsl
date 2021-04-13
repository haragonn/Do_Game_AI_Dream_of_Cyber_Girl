#include "common.hlsli"

struct VS_IN {
	float4 position : POSITION0;
	float4 color : COLOR0;
	float2 texcoord : TEXCOORD0;
};

struct VS_OUT {
	float4 position : SV_POSITION;
	float4 color : COLOR0;
	float2 texcoord : TEXCOORD0;
};

VS_OUT main(VS_IN input, uint instanceID : SV_InstanceID)
{
	VS_OUT output;

	output.position = mul(float4(input.position.x + perInstanceData[instanceID].position.x, input.position.y + perInstanceData[instanceID].position.y, 0.0f, 1.0f), projection2D);
	output.color = input.color;
	output.texcoord = input.texcoord;

	return output;
}