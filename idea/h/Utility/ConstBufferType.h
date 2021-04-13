#ifndef INCLUDE_IDEA_CONSTBUFFERTYPE_H
#define INCLUDE_IDEA_CONSTBUFFERTYPE_H

#define WIN32_LEAN_AND_MEAN
#include <directxmath.h>

// 2D定数バッファ
struct ConstBuffer2D
{
	DirectX::XMFLOAT4X4 proj;
};

// インスタンシングストラクチャバッファ
struct PerInstanceData
{
	DirectX::XMFLOAT4 pos;
};

struct ConstBuffer3D
{
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 proj;
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT4 light;
};

// ワールド座標情報
struct WorldConstBuffer
{
	DirectX::XMFLOAT4X4 world;
};

// VP情報
struct VPConstBuffer
{
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};

// ライト情報
struct LightConstBuffer
{
	DirectX::XMFLOAT4 direction;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 ambient;
};

// カメラ情報
struct CameraConstBuffer
{
	DirectX::XMFLOAT4 cameraPosition;
};

struct ShadowConstBuffer
{
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 proj;
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT4 light;
	DirectX::XMFLOAT4X4 lightView;
	DirectX::XMFLOAT4X4 lightProj;
};

// マテリアル情報
struct MaterialConstBuffer
{
	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular;
	DirectX::XMFLOAT4 emission;
	float shininess;
	float dummy[3];
};

// ボーン情報
struct BoneWorldConstBuffer
{
	DirectX::XMFLOAT4X4 boneWorld[512];
};

#endif	// #ifndef INCLUDE_IDEA_CONSTBUFFERTYPE_H