/*==============================================================================
[Cube.cpp]
Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <DirectXMath.h>
#include "../../h/3D/Cube.h"
#include "../../h/3D/ObjectManager.h"
#include "../../h/3D/3DStruct.h"
#include "../../h/Utility/ConstBufferType.h"
#include "../../h/Environment/Camera.h"
#include "../../h/Environment/ShadowCamera.h"
#include "../../h/Framework/GraphicManager.h"
#include "../../h/Texture/Texture.h"

//------------------------------------------------------------------------------
// using namespace
//------------------------------------------------------------------------------
using namespace DirectX;
using namespace idea;

namespace{
	void SetConstBuffer(Matrix4x4& world, Camera* pCamera, Color& color)
	{
		GraphicManager& gm = GraphicManager::Instance();
		ObjectManager& om = ObjectManager::Instance();

		//�萔�o�b�t�@
		ConstBuffer3D cbuff;
		XMFLOAT4X4 matWorld;

		for(int i = 4 - 1; i >= 0; --i){
			for(int j = 4 - 1; j >= 0; --j){
				matWorld.m[i][j] = world.r[i][j];
			}
		}
		XMStoreFloat4x4(&cbuff.world, XMMatrixTranspose(XMLoadFloat4x4(&matWorld)));

		XMFLOAT4X4 matView;
		for(int i = 4 - 1; i >= 0; --i){
			for(int j = 4 - 1; j >= 0; --j){
				matView.m[i][j] = pCamera->GetViewMatrix().r[i][j];
			}
		}
		XMStoreFloat4x4(&cbuff.view, XMMatrixTranspose(XMLoadFloat4x4(&matView)));

		XMFLOAT4X4 matProj;
		for(int i = 4 - 1; i >= 0; --i){
			for(int j = 4 - 1; j >= 0; --j){
				matProj.m[i][j] = pCamera->GetProjectionMatrix().r[i][j];
			}
		}
		XMStoreFloat4x4(&cbuff.proj, XMMatrixTranspose(XMLoadFloat4x4(&matProj)));

		XMStoreFloat4(&cbuff.color, XMVectorSet(color.r, color.g, color.b, color.a));

		XMVECTOR light = {};

		light.m128_f32[0] = om.GetLight().x;
		light.m128_f32[1] = om.GetLight().y;
		light.m128_f32[2] = om.GetLight().z;

		XMStoreFloat4(&cbuff.light, light);

		// �萔�o�b�t�@���e�X�V
		gm.GetContextPtr()->UpdateSubresource(om.GetConstBufferPtr(), 0, NULL, &cbuff, 0, 0);

		// �萔�o�b�t�@
		UINT cb_slot = 1;
		ID3D11Buffer* cb[] = { om.GetConstBufferPtr() };
		gm.GetContextPtr()->VSSetConstantBuffers(cb_slot, 1, cb);
	}

	void SetShadowConstBuffer(Matrix4x4& world, Camera* pCamera, Color& color, ShadowCamera* pScmr)
	{
		GraphicManager& gm = GraphicManager::Instance();
		ObjectManager& om = ObjectManager::Instance();

		//�萔�o�b�t�@
		ShadowConstBuffer cbuff;
		XMFLOAT4X4 matWorld;

		for(int i = 4 - 1; i >= 0; --i){
			for(int j = 4 - 1; j >= 0; --j){
				matWorld.m[i][j] = world.r[i][j];
			}
		}
		XMStoreFloat4x4(&cbuff.world, XMMatrixTranspose(XMLoadFloat4x4(&matWorld)));

		XMFLOAT4X4 matView;
		for(int i = 4 - 1; i >= 0; --i){
			for(int j = 4 - 1; j >= 0; --j){
				matView.m[i][j] = pCamera->GetViewMatrix().r[i][j];
			}
		}
		XMStoreFloat4x4(&cbuff.view, XMMatrixTranspose(XMLoadFloat4x4(&matView)));

		XMFLOAT4X4 matProj;
		for(int i = 4 - 1; i >= 0; --i){
			for(int j = 4 - 1; j >= 0; --j){
				matProj.m[i][j] = pCamera->GetProjectionMatrix().r[i][j];
			}
		}
		XMStoreFloat4x4(&cbuff.proj, XMMatrixTranspose(XMLoadFloat4x4(&matProj)));

		XMStoreFloat4(&cbuff.color, XMVectorSet(color.r, color.g, color.b, color.a));

		XMVECTOR light = {};

		light.m128_f32[0] = om.GetLight().x;
		light.m128_f32[1] = om.GetLight().y;
		light.m128_f32[2] = om.GetLight().z;

		XMStoreFloat4(&cbuff.light, light);

		XMFLOAT4X4 matLightView;
		for(int i = 4 - 1; i >= 0; --i){
			for(int j = 4 - 1; j >= 0; --j){
				matLightView.m[i][j] = pScmr->GetViewMatrix().r[i][j];
			}
		}
		XMStoreFloat4x4(&cbuff.lightView, XMMatrixTranspose(XMLoadFloat4x4(&matLightView)));

		XMFLOAT4X4 matLightProj;
		for(int i = 4 - 1; i >= 0; --i){
			for(int j = 4 - 1; j >= 0; --j){
				matLightProj.m[i][j] = pScmr->GetProjectionMatrix().r[i][j];
			}
		}
		XMStoreFloat4x4(&cbuff.lightProj, XMMatrixTranspose(XMLoadFloat4x4(&matLightProj)));

		// �萔�o�b�t�@���e�X�V
		gm.GetContextPtr()->UpdateSubresource(om.GetShadowConstBufferPtr(), 0, NULL, &cbuff, 0, 0);

		// �萔�o�b�t�@
		UINT cb_slot = 1;
		ID3D11Buffer* cb[] = { om.GetShadowConstBufferPtr() };
		gm.GetContextPtr()->VSSetConstantBuffers(cb_slot, 1, cb);
	}
}

Cube::Cube() :
	uNum_(1),
	vNum_(1),
	bDivided_(false),
	bDelimited_(false),
	pScmr_(nullptr){}

BoxCollider Cube::GetCollider()
{
	bc_.SetOBB(world_, prePosition_);

	return bc_;
}

BaseCollider* Cube::GetColliderPtr()
{
	bc_.SetOBB(world_, prePosition_);

	return &bc_;
}

void Cube::SetTexture(Texture& tex)
{
	TextureHolder::SetTexture(&tex);
	bDivided_ = false;
	bDelimited_ = false;
	uNum_ = 1;
	vNum_ = 1;
}

void Cube::SetDividedTexture(Texture& tex, int uNum, int vNum)
{
	TextureHolder::SetTexture(&tex);
	bDivided_ = true;
	bDelimited_ = false;
	uNum_ = uNum;
	vNum_ = vNum;
}

void Cube::SetDelimitedTexture(Texture& tex, float u, float v, float width, float height)
{
	TextureHolder::SetTexture(&tex);
	bDivided_ = false;
	bDelimited_ = true;
	uv_.x = u;
	uv_.y = v;
	size_.x = width;
	size_.y = height;
}

void Cube::ExclusionTexture()
{
	TextureHolder::ExclusionTexture();
	bDivided_ = false;
	bDelimited_ = false;
	uNum_ = 1;
	vNum_ = 1;
}

void Cube::SetShadow(ShadowCamera& scmr)
{
	pScmr_ = &scmr;
}

void Cube::Draw(Camera* pCamera)
{
	if(pScmr_ && !pScmr_->GetShadowDrawFlag()){
		if(!pTexture_){
			DrawShadowCube(pCamera);
		} else if(bDivided_){
			DrawDividedTextureShadowCube(pCamera, *pTexture_, uNum_, vNum_);
		} else if(bDelimited_){
			DrawDelimitedTextureShadowCube(pCamera, *pTexture_, uv_.x, uv_.y, size_.x, size_.y);
		} else{
			DrawTextureShadowCube(pCamera, *pTexture_);
		}
	} else{
		if(!pTexture_){
			DrawCube(pCamera);
		} else if(bDivided_){
			DrawDividedTextureCube(pCamera, *pTexture_, uNum_, vNum_);
		} else if(bDelimited_){
			DrawDelimitedTextureCube(pCamera, *pTexture_, uv_.x, uv_.y, size_.x, size_.y);
		} else{
			DrawTextureCube(pCamera, *pTexture_);
		}
	}
}

void Cube::DrawCube(Camera* pCamera, int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();

	//�萔�o�b�t�@
	SetConstBuffer(world_, pCamera, color_);

	// �o�b�t�@��������
	ID3D11Buffer* pVBuf = om.GetCubeVertexBufferPtr();

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(MeshVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// �C���f�b�N�X�o�b�t�@�̃Z�b�g
	gm.GetContextPtr()->IASetIndexBuffer(om.GetCubeIndexBufferPtr(), DXGI_FORMAT_R16_UINT, 0);

	// ���̓��C�A�E�g�̃Z�b�g
	gm.GetContextPtr()->IASetInputLayout(om.GetInputLayoutPtr());

	// �v���~�e�B�u�`��̃Z�b�g
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ���X�^���C�U�X�e�[�g
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// �f�v�X�X�e���V���X�e�[�g
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	// �V�F�[�_�̃Z�b�g
	gm.GetContextPtr()->VSSetShader(om.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	//�|���S���`��
	gm.GetContextPtr()->DrawIndexed(om.GetCubeIndexSize(), 0, 0);
}

inline void Cube::DrawShadowCube(Camera* pCamera, int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();

	//�萔�o�b�t�@
	SetShadowConstBuffer(world_, pCamera, color_, pScmr_);

	// �o�b�t�@��������
	ID3D11Buffer* pVBuf = om.GetCubeVertexBufferPtr();

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(MeshVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// �C���f�b�N�X�o�b�t�@�̃Z�b�g
	gm.GetContextPtr()->IASetIndexBuffer(om.GetCubeIndexBufferPtr(), DXGI_FORMAT_R16_UINT, 0);

	// ���̓��C�A�E�g�̃Z�b�g
	gm.GetContextPtr()->IASetInputLayout(om.GetShadowInputLayoutPtr());

	// �v���~�e�B�u�`��̃Z�b�g
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ���X�^���C�U�X�e�[�g
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// �f�v�X�X�e���V���X�e�[�g
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	ID3D11ShaderResourceView* const pSRV[1] = { NULL };
	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);

	// �e�N�X�`����������
	ID3D11ShaderResourceView* pTexView = gm.GetShaderResourceViewPtrArray()[1];

	if(pTexView){
		gm.GetContextPtr()->PSSetShaderResources(1, 1, &pTexView);
	}

	// �V�F�[�_�̃Z�b�g
	gm.GetContextPtr()->VSSetShader(om.GetShadowVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->PSSetShader(om.GetPixelShaderShadowPtr(), NULL, 0);
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	//�|���S���`��
	gm.GetContextPtr()->DrawIndexed(om.GetCubeIndexSize(), 0, 0);

	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);
}

void Cube::DrawTextureCube(Camera* pCamera, const Texture& tex, int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();

	//�萔�o�b�t�@
	SetConstBuffer(world_, pCamera, color_);

	MeshVertexData vertexList[]{
	{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, color_, { 0.0f, 0.0f } },
	{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, color_, { 1.0f, 0.0f } },
	{ { -0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, color_, { 0.0f, 1.0f } },
	{ {  0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, color_, { 1.0f, 1.0f } },

	{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, color_, { 1.0f, 0.0f } },
	{ { -0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, color_, { 1.0f, 1.0f } },
	{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, color_, { 0.0f, 0.0f } },
	{ {  0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, color_, { 0.0f, 1.0f } },

	{ { -0.5f,  0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, color_, { 0.0f, 0.0f } },
	{ { -0.5f,  0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, color_, { 1.0f, 0.0f } },
	{ { -0.5f, -0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, color_, { 0.0f, 1.0f } },
	{ { -0.5f, -0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, color_, { 1.0f, 1.0f } },

	{ {  0.5f,  0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, color_, { 1.0f, 0.0f } },
	{ {  0.5f, -0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, color_, { 1.0f, 1.0f } },
	{ {  0.5f,  0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, color_, { 0.0f, 0.0f } },
	{ {  0.5f, -0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, color_, { 0.0f, 1.0f } },

	{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, color_, { 0.0f, 0.0f } },
	{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, color_, { 1.0f, 0.0f } },
	{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, color_, { 0.0f, 1.0f } },
	{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, color_, { 1.0f, 1.0f } },

	{ { -0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, color_, { 1.0f, 0.0f } },
	{ { -0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, color_, { 1.0f, 1.0f } },
	{ {  0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, color_, { 0.0f, 0.0f } },
	{ {  0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, color_, { 0.0f, 1.0f } },
	};

	// �o�b�t�@��������
	ID3D11Buffer* pVBuf = om.GetCubeVertexBufferPtr();
	D3D11_MAPPED_SUBRESOURCE msr;
	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vertexList, sizeof(MeshVertexData) * ObjectManager::CUBE_VERTEX_NUM);
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(MeshVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// �C���f�b�N�X�o�b�t�@�̃Z�b�g
	gm.GetContextPtr()->IASetIndexBuffer(om.GetCubeIndexBufferPtr(), DXGI_FORMAT_R16_UINT, 0);

	// �e�N�X�`����������
	ID3D11ShaderResourceView* pTexView = tex.GetTextureViewPtr();
	if(pTexView){
		gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
	}

	// ���̓��C�A�E�g�̃Z�b�g
	gm.GetContextPtr()->IASetInputLayout(om.GetInputLayoutPtr());

	// �v���~�e�B�u�`��̃Z�b�g
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ���X�^���C�U�X�e�[�g
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// �f�v�X�X�e���V���X�e�[�g
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	// �V�F�[�_�̃Z�b�g
	gm.GetContextPtr()->VSSetShader(om.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	if(pTexView){
		gm.GetContextPtr()->PSSetShader(om.GetPixelShederTexturePtr(), NULL, 0);
	} else{
		gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
	}
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	//�|���S���`��
	gm.GetContextPtr()->DrawIndexed(om.GetCubeIndexSize(), 0, 0);
}

inline void Cube::DrawTextureShadowCube(Camera* pCamera, const Texture& tex, int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();

	//�萔�o�b�t�@
	SetShadowConstBuffer(world_, pCamera, color_, pScmr_);

	MeshVertexData vertexList[]{
	{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, color_, { 0.0f, 0.0f } },
	{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, color_, { 1.0f, 0.0f } },
	{ { -0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, color_, { 0.0f, 1.0f } },
	{ {  0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, color_, { 1.0f, 1.0f } },

	{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, color_, { 1.0f, 0.0f } },
	{ { -0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, color_, { 1.0f, 1.0f } },
	{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, color_, { 0.0f, 0.0f } },
	{ {  0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, color_, { 0.0f, 1.0f } },

	{ { -0.5f,  0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, color_, { 0.0f, 0.0f } },
	{ { -0.5f,  0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, color_, { 1.0f, 0.0f } },
	{ { -0.5f, -0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, color_, { 0.0f, 1.0f } },
	{ { -0.5f, -0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, color_, { 1.0f, 1.0f } },

	{ {  0.5f,  0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, color_, { 1.0f, 0.0f } },
	{ {  0.5f, -0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, color_, { 1.0f, 1.0f } },
	{ {  0.5f,  0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, color_, { 0.0f, 0.0f } },
	{ {  0.5f, -0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, color_, { 0.0f, 1.0f } },

	{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, color_, { 0.0f, 0.0f } },
	{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, color_, { 1.0f, 0.0f } },
	{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, color_, { 0.0f, 1.0f } },
	{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, color_, { 1.0f, 1.0f } },

	{ { -0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, color_, { 1.0f, 0.0f } },
	{ { -0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, color_, { 1.0f, 1.0f } },
	{ {  0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, color_, { 0.0f, 0.0f } },
	{ {  0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, color_, { 0.0f, 1.0f } },
	};

	// �o�b�t�@��������
	ID3D11Buffer* pVBuf = om.GetCubeVertexBufferPtr();
	D3D11_MAPPED_SUBRESOURCE msr;
	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vertexList, sizeof(MeshVertexData) * ObjectManager::CUBE_VERTEX_NUM);
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(MeshVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// �C���f�b�N�X�o�b�t�@�̃Z�b�g
	gm.GetContextPtr()->IASetIndexBuffer(om.GetCubeIndexBufferPtr(), DXGI_FORMAT_R16_UINT, 0);

	// ���̓��C�A�E�g�̃Z�b�g
	gm.GetContextPtr()->IASetInputLayout(om.GetShadowInputLayoutPtr());

	// �v���~�e�B�u�`��̃Z�b�g
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ���X�^���C�U�X�e�[�g
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// �f�v�X�X�e���V���X�e�[�g
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	ID3D11ShaderResourceView* const pSRV[1] = { NULL };
	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);

	// �e�N�X�`����������
	{
		ID3D11ShaderResourceView* pTexView = tex.GetTextureViewPtr();
		if(pTexView){
			gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
		}
	}

	{
		ID3D11ShaderResourceView* pTexView = gm.GetShaderResourceViewPtrArray()[1];
		if(pTexView){
			gm.GetContextPtr()->PSSetShaderResources(1, 1, &pTexView);
		}
	}

	// �V�F�[�_�̃Z�b�g
	gm.GetContextPtr()->VSSetShader(om.GetShadowVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	if(tex.GetTextureViewPtr()){
		gm.GetContextPtr()->PSSetShader(om.GetPixelShaderTextureShadowPtr(), NULL, 0);
	} else{
		gm.GetContextPtr()->PSSetShader(om.GetPixelShaderShadowPtr(), NULL, 0);
	}
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	//�|���S���`��
	gm.GetContextPtr()->DrawIndexed(om.GetCubeIndexSize(), 0, 0);

	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);
}

void Cube::DrawDividedTextureCube(Camera* pCamera, const Texture& tex, int uNum, int vNum, int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();

	//�萔�o�b�t�@
	SetConstBuffer(world_, pCamera, color_);

	// ���_�o�b�t�@
	float u1 = tex.GetDivU() * uNum;
	float u2 = tex.GetDivU() * (uNum + 1);
	float v1 = tex.GetDivV() * vNum;
	float v2 = tex.GetDivV() * (vNum + 1);

	MeshVertexData vertexList[]{
	{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, color_, { u1, v1 } },
	{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, color_, { u2, v1 } },
	{ { -0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, color_, { u1, v2 } },
	{ {  0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, color_, { u2, v2 } },

	{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, color_, { u2, v1 } },
	{ { -0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, color_, { u2, v2 } },
	{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, color_, { u1, v1 } },
	{ {  0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, color_, { u1, v2 } },

	{ { -0.5f,  0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, color_, { u1, v1 } },
	{ { -0.5f,  0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, color_, { u2, v1 } },
	{ { -0.5f, -0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, color_, { u1, v2 } },
	{ { -0.5f, -0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, color_, { u2, v2 } },

	{ {  0.5f,  0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, color_, { u2, v1 } },
	{ {  0.5f, -0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, color_, { u2, v2 } },
	{ {  0.5f,  0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, color_, { u1, v1 } },
	{ {  0.5f, -0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, color_, { u1, v2 } },

	{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, color_, { u1, v1 } },
	{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, color_, { u2, v1 } },
	{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, color_, { u1, v2 } },
	{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, color_, { u2, v2 } },

	{ { -0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, color_, { u2, v1 } },
	{ { -0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, color_, { u2, v2 } },
	{ {  0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, color_, { u1, v1 } },
	{ {  0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, color_, { u1, v2 } },
	};

	// �o�b�t�@��������
	ID3D11Buffer* pVBuf = om.GetCubeVertexBufferPtr();
	D3D11_MAPPED_SUBRESOURCE msr;
	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vertexList, sizeof(MeshVertexData) * ObjectManager::CUBE_VERTEX_NUM);
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(MeshVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// �C���f�b�N�X�o�b�t�@�̃Z�b�g
	gm.GetContextPtr()->IASetIndexBuffer(om.GetCubeIndexBufferPtr(), DXGI_FORMAT_R16_UINT, 0);

	// �e�N�X�`����������
	ID3D11ShaderResourceView* pTexView = tex.GetTextureViewPtr();
	if(pTexView){
		gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
	}

	// ���̓��C�A�E�g�̃Z�b�g
	gm.GetContextPtr()->IASetInputLayout(om.GetInputLayoutPtr());

	// �v���~�e�B�u�`��̃Z�b�g
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ���X�^���C�U�X�e�[�g
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// �f�v�X�X�e���V���X�e�[�g
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	// �V�F�[�_�̃Z�b�g
	gm.GetContextPtr()->VSSetShader(om.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	if(pTexView){
		gm.GetContextPtr()->PSSetShader(om.GetPixelShederTexturePtr(), NULL, 0);
	} else{
		gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
	}
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	//�|���S���`��
	gm.GetContextPtr()->DrawIndexed(om.GetCubeIndexSize(), 0, 0);
}

inline void Cube::DrawDividedTextureShadowCube(Camera* pCamera, const Texture& tex, int uNum, int vNum, int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();

	//�萔�o�b�t�@
	SetShadowConstBuffer(world_, pCamera, color_, pScmr_);

	// ���_�o�b�t�@
	float u1 = tex.GetDivU() * uNum;
	float u2 = tex.GetDivU() * (uNum + 1);
	float v1 = tex.GetDivV() * vNum;
	float v2 = tex.GetDivV() * (vNum + 1);

	MeshVertexData vertexList[]{
	{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, color_, { u1, v1 } },
	{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, color_, { u2, v1 } },
	{ { -0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, color_, { u1, v2 } },
	{ {  0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, color_, { u2, v2 } },

	{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, color_, { u2, v1 } },
	{ { -0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, color_, { u2, v2 } },
	{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, color_, { u1, v1 } },
	{ {  0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, color_, { u1, v2 } },

	{ { -0.5f,  0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, color_, { u1, v1 } },
	{ { -0.5f,  0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, color_, { u2, v1 } },
	{ { -0.5f, -0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, color_, { u1, v2 } },
	{ { -0.5f, -0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, color_, { u2, v2 } },

	{ {  0.5f,  0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, color_, { u2, v1 } },
	{ {  0.5f, -0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, color_, { u2, v2 } },
	{ {  0.5f,  0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, color_, { u1, v1 } },
	{ {  0.5f, -0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, color_, { u1, v2 } },

	{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, color_, { u1, v1 } },
	{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, color_, { u2, v1 } },
	{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, color_, { u1, v2 } },
	{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, color_, { u2, v2 } },

	{ { -0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, color_, { u2, v1 } },
	{ { -0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, color_, { u2, v2 } },
	{ {  0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, color_, { u1, v1 } },
	{ {  0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, color_, { u1, v2 } },
	};

	// �o�b�t�@��������
	ID3D11Buffer* pVBuf = om.GetCubeVertexBufferPtr();
	D3D11_MAPPED_SUBRESOURCE msr;
	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vertexList, sizeof(MeshVertexData) * ObjectManager::CUBE_VERTEX_NUM);
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(MeshVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// �C���f�b�N�X�o�b�t�@�̃Z�b�g
	gm.GetContextPtr()->IASetIndexBuffer(om.GetCubeIndexBufferPtr(), DXGI_FORMAT_R16_UINT, 0);

	// ���̓��C�A�E�g�̃Z�b�g
	gm.GetContextPtr()->IASetInputLayout(om.GetShadowInputLayoutPtr());

	// �v���~�e�B�u�`��̃Z�b�g
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ���X�^���C�U�X�e�[�g
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// �f�v�X�X�e���V���X�e�[�g
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	ID3D11ShaderResourceView* const pSRV[1] = { NULL };
	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);

	// �e�N�X�`����������
	{
		ID3D11ShaderResourceView* pTexView = tex.GetTextureViewPtr();
		if(pTexView){
			gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
		}
	}

	{
		ID3D11ShaderResourceView* pTexView = gm.GetShaderResourceViewPtrArray()[1];
		if(pTexView){
			gm.GetContextPtr()->PSSetShaderResources(1, 1, &pTexView);
		}
	}

	// �V�F�[�_�̃Z�b�g
	gm.GetContextPtr()->VSSetShader(om.GetShadowVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	if(tex.GetTextureViewPtr()){
		gm.GetContextPtr()->PSSetShader(om.GetPixelShaderTextureShadowPtr(), NULL, 0);
	} else{
		gm.GetContextPtr()->PSSetShader(om.GetPixelShaderShadowPtr(), NULL, 0);
	}
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	//�|���S���`��
	gm.GetContextPtr()->DrawIndexed(om.GetCubeIndexSize(), 0, 0);

	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);
}

inline void Cube::DrawDelimitedTextureCube(Camera* pCamera, const Texture& tex, float u, float v, float width, float height, int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();

	//�萔�o�b�t�@
	SetConstBuffer(world_, pCamera, color_);

	// ���_�o�b�t�@
	float u1 = u;
	float u2 = u + width;
	float v1 = v;
	float v2 = v + height;

	MeshVertexData vertexList[]{
	{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, color_, { u1, v1 } },
	{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, color_, { u2, v1 } },
	{ { -0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, color_, { u1, v2 } },
	{ {  0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, color_, { u2, v2 } },

	{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, color_, { u2, v1 } },
	{ { -0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, color_, { u2, v2 } },
	{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, color_, { u1, v1 } },
	{ {  0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, color_, { u1, v2 } },

	{ { -0.5f,  0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, color_, { u1, v1 } },
	{ { -0.5f,  0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, color_, { u2, v1 } },
	{ { -0.5f, -0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, color_, { u1, v2 } },
	{ { -0.5f, -0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, color_, { u2, v2 } },

	{ {  0.5f,  0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, color_, { u2, v1 } },
	{ {  0.5f, -0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, color_, { u2, v2 } },
	{ {  0.5f,  0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, color_, { u1, v1 } },
	{ {  0.5f, -0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, color_, { u1, v2 } },

	{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, color_, { u1, v1 } },
	{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, color_, { u2, v1 } },
	{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, color_, { u1, v2 } },
	{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, color_, { u2, v2 } },

	{ { -0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, color_, { u2, v1 } },
	{ { -0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, color_, { u2, v2 } },
	{ {  0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, color_, { u1, v1 } },
	{ {  0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, color_, { u1, v2 } },
	};

	// �o�b�t�@��������
	ID3D11Buffer* pVBuf = om.GetCubeVertexBufferPtr();
	D3D11_MAPPED_SUBRESOURCE msr;
	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vertexList, sizeof(MeshVertexData) * ObjectManager::CUBE_VERTEX_NUM);
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(MeshVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// �C���f�b�N�X�o�b�t�@�̃Z�b�g
	gm.GetContextPtr()->IASetIndexBuffer(om.GetCubeIndexBufferPtr(), DXGI_FORMAT_R16_UINT, 0);

	// �e�N�X�`����������
	ID3D11ShaderResourceView* pTexView = tex.GetTextureViewPtr();
	if(pTexView){
		gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
	}

	// ���̓��C�A�E�g�̃Z�b�g
	gm.GetContextPtr()->IASetInputLayout(om.GetInputLayoutPtr());

	// �v���~�e�B�u�`��̃Z�b�g
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ���X�^���C�U�X�e�[�g
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// �f�v�X�X�e���V���X�e�[�g
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	// �V�F�[�_�̃Z�b�g
	gm.GetContextPtr()->VSSetShader(om.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	if(pTexView){
		gm.GetContextPtr()->PSSetShader(om.GetPixelShederTexturePtr(), NULL, 0);
	} else{
		gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
	}
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	//�|���S���`��
	gm.GetContextPtr()->DrawIndexed(om.GetCubeIndexSize(), 0, 0);
}

inline void Cube::DrawDelimitedTextureShadowCube(Camera* pCamera, const Texture& tex, float u, float v, float width, float height, int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();

	//�萔�o�b�t�@
	SetShadowConstBuffer(world_, pCamera, color_, pScmr_);

	// ���_�o�b�t�@
	float u1 = u;
	float u2 = u + width;
	float v1 = v;
	float v2 = v + height;

	MeshVertexData vertexList[]{
	{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, color_, { u1, v1 } },
	{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, color_, { u2, v1 } },
	{ { -0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, color_, { u1, v2 } },
	{ {  0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, color_, { u2, v2 } },

	{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, color_, { u2, v1 } },
	{ { -0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, color_, { u2, v2 } },
	{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, color_, { u1, v1 } },
	{ {  0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, color_, { u1, v2 } },

	{ { -0.5f,  0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, color_, { u1, v1 } },
	{ { -0.5f,  0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, color_, { u2, v1 } },
	{ { -0.5f, -0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, color_, { u1, v2 } },
	{ { -0.5f, -0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, color_, { u2, v2 } },

	{ {  0.5f,  0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, color_, { u2, v1 } },
	{ {  0.5f, -0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, color_, { u2, v2 } },
	{ {  0.5f,  0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, color_, { u1, v1 } },
	{ {  0.5f, -0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, color_, { u1, v2 } },

	{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, color_, { u1, v1 } },
	{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, color_, { u2, v1 } },
	{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, color_, { u1, v2 } },
	{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, color_, { u2, v2 } },

	{ { -0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, color_, { u2, v1 } },
	{ { -0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, color_, { u2, v2 } },
	{ {  0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, color_, { u1, v1 } },
	{ {  0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, color_, { u1, v2 } },
	};

	// �o�b�t�@��������
	ID3D11Buffer* pVBuf = om.GetCubeVertexBufferPtr();
	D3D11_MAPPED_SUBRESOURCE msr;
	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vertexList, sizeof(MeshVertexData) * ObjectManager::CUBE_VERTEX_NUM);
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(MeshVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// �C���f�b�N�X�o�b�t�@�̃Z�b�g
	gm.GetContextPtr()->IASetIndexBuffer(om.GetCubeIndexBufferPtr(), DXGI_FORMAT_R16_UINT, 0);

	// ���̓��C�A�E�g�̃Z�b�g
	gm.GetContextPtr()->IASetInputLayout(om.GetShadowInputLayoutPtr());

	// �v���~�e�B�u�`��̃Z�b�g
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ���X�^���C�U�X�e�[�g
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// �f�v�X�X�e���V���X�e�[�g
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	ID3D11ShaderResourceView* const pSRV[1] = { NULL };
	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);

	// �e�N�X�`����������
	{
		ID3D11ShaderResourceView* pTexView = tex.GetTextureViewPtr();
		if(pTexView){
			gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
		}
	}

	{
		ID3D11ShaderResourceView* pTexView = gm.GetShaderResourceViewPtrArray()[1];
		if(pTexView){
			gm.GetContextPtr()->PSSetShaderResources(1, 1, &pTexView);
		}
	}

	// �V�F�[�_�̃Z�b�g
	gm.GetContextPtr()->VSSetShader(om.GetShadowVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	if(tex.GetTextureViewPtr()){
		gm.GetContextPtr()->PSSetShader(om.GetPixelShaderTextureShadowPtr(), NULL, 0);
	} else{
		gm.GetContextPtr()->PSSetShader(om.GetPixelShaderShadowPtr(), NULL, 0);
	}
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// �r���[�|�[�g�̐ݒ�
	//SetViewPort(pCamera);

	//�|���S���`��
	gm.GetContextPtr()->DrawIndexed(om.GetCubeIndexSize(), 0, 0);

	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);
}