#ifndef INCLUDE_IDEA_CONSTBUFFERTYPE_H
#define INCLUDE_IDEA_CONSTBUFFERTYPE_H

#define WIN32_LEAN_AND_MEAN
#include <directxmath.h>

// 2D�萔�o�b�t�@
struct ConstBuffer2D
{
	DirectX::XMFLOAT4X4 proj;
};

// �C���X�^���V���O�X�g���N�`���o�b�t�@
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

// ���[���h���W���
struct WorldConstBuffer
{
	DirectX::XMFLOAT4X4 world;
};

// VP���
struct VPConstBuffer
{
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};

// ���C�g���
struct LightConstBuffer
{
	DirectX::XMFLOAT4 direction;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 ambient;
};

// �J�������
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

// �}�e���A�����
struct MaterialConstBuffer
{
	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular;
	DirectX::XMFLOAT4 emission;
	float shininess;
	float dummy[3];
};

// �{�[�����
struct BoneWorldConstBuffer
{
	DirectX::XMFLOAT4X4 boneWorld[512];
};

#endif	// #ifndef INCLUDE_IDEA_CONSTBUFFERTYPE_H