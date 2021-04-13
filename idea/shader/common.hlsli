struct LIGHT {
	float4 direction;
	float4 diffuse;
	float4 ambient;
	//matrix lightView;  // ���C�g�r���[�ϊ��s��
	//matrix lightProjection;  // �ˉe�ϊ��s��
};

struct MATERIAL {
	float4		ambient;
	float4		diffuse;
	float4		specular;
	float4		emission;
	float		shininess;
	float3		dummy;
};

struct PerInstanceData{
	float4 position;
};

cbuffer Projection2DBuffer : register(b0)
{
	matrix projection2D;
}

cbuffer WorldBuffer : register(b1) {
	matrix world;
}

cbuffer VPBuffer : register(b2) {
	matrix projection;
	matrix view;
}

cbuffer LightBuffer : register(b3) {
	LIGHT light;
}

cbuffer CameraBuffer : register(b4) {
	float4 cameraPosition;
}

cbuffer MaterialBuffer : register(b5) {
	MATERIAL material;
}

cbuffer BoneBuffer : register(b6)
{
	matrix boneWorld[512];
}

Texture2D<float4> myTexture : register(t0);

Texture2D<float> shadowTexture : register(t1);

StructuredBuffer<PerInstanceData> perInstanceData : register(t2);

SamplerState mySampler : register(s0);