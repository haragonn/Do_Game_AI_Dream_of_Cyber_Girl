/*==============================================================================
[Sphere.cpp]
Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <DirectXMath.h>
#include "../../h/3D/Sphere.h"
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

Sphere::Sphere() : 
	pScmr_(nullptr){}

SphereCollider Sphere::GetCollider()
{
	Vector3D scale = ScalingFromMatrix4x4(world_);

	sc_.SetSphere(TransformFromMatrix4x4(world_), Max(scale.x, Max(scale.y, scale.z)) * 0.5f, prePosition_);

	return sc_;
}

BaseCollider* idea::Sphere::GetColliderPtr()
{
	Vector3D scale = ScalingFromMatrix4x4(world_);

	sc_.SetSphere(TransformFromMatrix4x4(world_), Max(scale.x, Max(scale.y, scale.z)) * 0.5f, prePosition_);

	return &sc_;
}

void Sphere::SetTexture(Texture& tex)
{
	TextureHolder::SetTexture(&tex);
}

void Sphere::ExclusionTexture()
{
	TextureHolder::ExclusionTexture();
}

void Sphere::SetShadow(ShadowCamera& scmr)
{
	pScmr_ = &scmr;
}

void Sphere::Draw(Camera* pCamera)
{
	if(pScmr_ && !pScmr_->GetShadowDrawFlag()){
		if(!pTexture_){
			DrawShadowSphere(pCamera);
		} else{
			DrawTextureShadowSphere(pCamera, *pTexture_);
		}
	} else{
		if(!pTexture_){
			DrawSphere(pCamera);
		} else{
			DrawTextureSphere(pCamera, *pTexture_);
		}
	}
}

inline void Sphere::DrawSphere(Camera* pCamera, int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();

	//�萔�o�b�t�@
	SetConstBuffer(world_, pCamera, color_);

	// �o�b�t�@��������
	ID3D11Buffer* pVBuf = om.GetSphereVertexBufferPtr();

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(MeshVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// �C���f�b�N�X�o�b�t�@�̃Z�b�g
	gm.GetContextPtr()->IASetIndexBuffer(om.GetSphereIndexBufferPtr(), DXGI_FORMAT_R16_UINT, 0);

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
	gm.GetContextPtr()->DrawIndexed(om.GetSphereIndexSize(), 0, 0);
}

inline void Sphere::DrawShadowSphere(Camera* pCamera, int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();

	//�萔�o�b�t�@
	SetShadowConstBuffer(world_, pCamera, color_, pScmr_);

	// �o�b�t�@��������
	ID3D11Buffer* pVBuf = om.GetSphereVertexBufferPtr();

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(MeshVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// �C���f�b�N�X�o�b�t�@�̃Z�b�g
	gm.GetContextPtr()->IASetIndexBuffer(om.GetSphereIndexBufferPtr(), DXGI_FORMAT_R16_UINT, 0);

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
	gm.GetContextPtr()->DrawIndexed(om.GetSphereIndexSize(), 0, 0);

	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);
}

inline void Sphere::DrawTextureSphere(Camera* pCamera, const Texture& tex, int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();

	//�萔�o�b�t�@
	SetConstBuffer(world_, pCamera, color_);

	// �o�b�t�@��������
	ID3D11Buffer* pVBuf = om.GetSphereVertexBufferPtr();

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(MeshVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// �C���f�b�N�X�o�b�t�@�̃Z�b�g
	gm.GetContextPtr()->IASetIndexBuffer(om.GetSphereIndexBufferPtr(), DXGI_FORMAT_R16_UINT, 0);

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
	gm.GetContextPtr()->DrawIndexed(om.GetSphereIndexSize(), 0, 0);
}

inline void Sphere::DrawTextureShadowSphere(Camera* pCamera, const Texture& tex, int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();

	//�萔�o�b�t�@
	SetShadowConstBuffer(world_, pCamera, color_, pScmr_);

	// �o�b�t�@��������
	ID3D11Buffer* pVBuf = om.GetSphereVertexBufferPtr();

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(MeshVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// �C���f�b�N�X�o�b�t�@�̃Z�b�g
	gm.GetContextPtr()->IASetIndexBuffer(om.GetSphereIndexBufferPtr(), DXGI_FORMAT_R16_UINT, 0);

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
	gm.GetContextPtr()->DrawIndexed(om.GetSphereIndexSize(), 0, 0);

	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);
}
