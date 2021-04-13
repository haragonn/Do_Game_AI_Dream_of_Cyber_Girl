/*==============================================================================
[Billboard.cpp]
Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <DirectXMath.h>
#include "../../h/3D/Billboard.h"
#include "../../h/3D/ObjectManager.h"
#include "../../h/3D/3DStruct.h"
#include "../../h/Utility/ConstBufferType.h"
#include "../../h/Environment/Camera.h"
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

			//定数バッファ
			ConstBuffer3D cbuff = {};

			XMFLOAT4X4 matWorld = {};
			for(int i = 4 - 1; i >= 0; --i){
				for(int j = 4 - 1; j >= 0; --j){
					matWorld.m[i][j] = world.r[i][j];
				}
			}
			XMStoreFloat4x4(&cbuff.world, XMMatrixTranspose(XMLoadFloat4x4(&matWorld)));

			XMFLOAT4X4 matView = {};
			for(int i = 4 - 1; i >= 0; --i){
				for(int j = 4 - 1; j >= 0; --j){
					matView.m[i][j] = pCamera->GetViewMatrix().r[i][j];
				}
			}
			XMStoreFloat4x4(&cbuff.view, XMMatrixTranspose(XMLoadFloat4x4(&matView)));

			XMFLOAT4X4 matProj = {};
			for(int i = 4 - 1; i >= 0; --i){
				for(int j = 4 - 1; j >= 0; --j){
					matProj.m[i][j] = pCamera->GetProjectionMatrix().r[i][j];
				}
			}
			XMStoreFloat4x4(&cbuff.proj, XMMatrixTranspose(XMLoadFloat4x4(&matProj)));

			XMStoreFloat4(&cbuff.color, XMVectorSet(color.r, color.g, color.b, color.a));

			XMVECTOR lightVec = XMVector3Normalize(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));

			XMStoreFloat4(&cbuff.light, lightVec);

			// 定数バッファ内容更新
			gm.GetContextPtr()->UpdateSubresource(om.GetConstBufferPtr(), 0, NULL, &cbuff, 0, 0);

			// 定数バッファ
			UINT cb_slot = 1;
			ID3D11Buffer* cb[] = { om.GetConstBufferPtr() };
			gm.GetContextPtr()->VSSetConstantBuffers(cb_slot, 1, cb);
	}

	void SetMaterialConstBuffer(Material& material)
	{
		GraphicManager& gm = GraphicManager::Instance();
		ObjectManager& om = ObjectManager::Instance();

		//定数バッファ
		MaterialConstBuffer cbuff = {};

		XMStoreFloat4(&cbuff.ambient, XMVectorSet(material.ambient.r, material.ambient.g, material.ambient.b, material.ambient.a));
		XMStoreFloat4(&cbuff.diffuse, XMVectorSet(material.diffuse.r, material.diffuse.g, material.diffuse.b, material.diffuse.a));
		XMStoreFloat4(&cbuff.specular, XMVectorSet(material.specular.r, material.specular.g, material.specular.b, material.specular.a));
		cbuff.shininess = material.shininess;

		// 定数バッファ内容更新
		gm.GetContextPtr()->UpdateSubresource(om.GetMaterialConstBufferPtr(), 0, NULL, &cbuff, 0, 0);

		// 定数バッファ
		UINT cb_slot = 2;
		ID3D11Buffer* cb[] = { om.GetMaterialConstBufferPtr() };
		gm.GetContextPtr()->VSSetConstantBuffers(cb_slot, 1, cb);
	}
}

idea::Billboard::Billboard() :
	uNum_(0),
	vNum_(0),
	bDivided_(false),
	blend_(0),
	bYBillboard_(false)
{}

void Billboard::SetTexture(Texture& tex)
{
	TextureHolder::SetTexture(&tex);
	bDivided_ = false;
	uNum_ = 0;
	vNum_ = 0;
}

void Billboard::SetDividedTexture(Texture& tex, int uNum, int vNum)
{
	TextureHolder::SetTexture(&tex);
	bDivided_ = true;
	uNum_ = uNum;
	vNum_ = vNum;
}

void Billboard::ExclusionTexture()
{
	TextureHolder::ExclusionTexture();
	bDivided_ = false;
	uNum_ = 0;
	vNum_ = 0;
}

void Billboard::Draw(Camera* pCamera)
{
	UpdateMatrix(pCamera);

	if(!pTexture_){
		DrawBillboard(pCamera, blend_);
	} else if(!bDivided_){
		DrawTextureBillboard(pCamera, *pTexture_, blend_);
	} else{
		DrawDividedTextureBillboard(pCamera, *pTexture_, uNum_, vNum_, blend_);
	}
}

inline void Billboard::DrawBillboard(Camera* pCamera, int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();

	//定数バッファ
	SetConstBuffer(world_, pCamera, color_);
	SetMaterialConstBuffer(material_);

	// バッファ書き込み
	ID3D11Buffer* pVBuf = om.GetBillboardVertexBufferPtr();

	// 頂点バッファのセット
	UINT stride = sizeof(BillboardVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// 入力レイアウトのセット
	gm.GetContextPtr()->IASetInputLayout(om.GetBillboardInputLayoutPtr());

	// プリミティブ形状のセット
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ブレンディングのセット
	if(!blend){
		gm.SetBlendState(BLEND_STATE::BLEND_ALIGNMENT);
	} else if(blend > 0){
		gm.SetBlendState(BLEND_STATE::BLEND_ADD);
	} else{
		gm.SetBlendState(BLEND_STATE::BLEND_SUBTRACT);
	}

	// ラスタライザステート
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// デプスステンシルステート
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	// シェーダのセット
	gm.GetContextPtr()->VSSetShader(om.GetBillboardVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	//ポリゴン描画
	gm.GetContextPtr()->Draw(4, 0);
	gm.SetBlendState(BLEND_STATE::BLEND_ALIGNMENT);
}

inline void Billboard::DrawTextureBillboard(Camera* pCamera, const Texture& tex, int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();

	//定数バッファ
	SetConstBuffer(world_, pCamera, color_);
	SetMaterialConstBuffer(material_);

	BillboardVertexData vertexList[] = {
	{ { -0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f } },
	{ {  0.5f,  0.5f, 0.0f }, { 1.0f, 0.0f } },
	{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f } },
	{ {  0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f } }
	};

	// バッファ書き込み
	ID3D11Buffer* pVBuf = om.GetBillboardVertexBufferPtr();
	D3D11_MAPPED_SUBRESOURCE msr;
	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vertexList, sizeof(BillboardVertexData) * 4);
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// 頂点バッファのセット
	UINT stride = sizeof(BillboardVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// テクスチャ書き込み
	ID3D11ShaderResourceView* pTexView = tex.GetTextureViewPtr();
	if(pTexView){
		gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
	}

	// 入力レイアウトのセット
	gm.GetContextPtr()->IASetInputLayout(om.GetBillboardInputLayoutPtr());

	// プリミティブ形状のセット
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ブレンディングのセット
	if(!blend){
		gm.SetBlendState(BLEND_STATE::BLEND_ALIGNMENT);
	} else if(blend > 0){
		gm.SetBlendState(BLEND_STATE::BLEND_ADD);
	} else{
		gm.SetBlendState(BLEND_STATE::BLEND_SUBTRACT);
	}

	// ラスタライザステート
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// デプスステンシルステート
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	// シェーダのセット
	gm.GetContextPtr()->VSSetShader(om.GetBillboardVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	if(pTexView){
		gm.GetContextPtr()->PSSetShader(om.GetPixelShederTexturePtr(), NULL, 0);
	} else{
		gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
	}

	//ポリゴン描画
	gm.GetContextPtr()->Draw(4, 0);
	gm.SetBlendState(BLEND_STATE::BLEND_ALIGNMENT);
}

inline void Billboard::DrawDividedTextureBillboard(Camera* pCamera, const Texture& tex, int uNum, int vNum, int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();

	//定数バッファ
	SetConstBuffer(world_, pCamera, color_);
	SetMaterialConstBuffer(material_);

	float u1 = tex.GetDivU() * uNum;
	float u2 = tex.GetDivU() * (uNum + 1);
	float v1 = tex.GetDivV() * vNum;
	float v2 = tex.GetDivV() * (vNum + 1);

	BillboardVertexData vertexList[] = {
	{ { -0.5f,  0.5f, 0.0f }, { u1, v1 } },
	{ {  0.5f,  0.5f, 0.0f }, { u2, v1 } },
	{ { -0.5f, -0.5f, 0.0f }, { u1, v2 } },
	{ {  0.5f, -0.5f, 0.0f }, { u2, v2 } }
	};

	// バッファ書き込み
	ID3D11Buffer* pVBuf = om.GetBillboardVertexBufferPtr();
	D3D11_MAPPED_SUBRESOURCE msr;
	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vertexList, sizeof(BillboardVertexData) * 4);
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// 頂点バッファのセット
	UINT stride = sizeof(BillboardVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// テクスチャ書き込み
	ID3D11ShaderResourceView* pTexView = tex.GetTextureViewPtr();
	if(pTexView){
		gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
	}

	// 入力レイアウトのセット
	gm.GetContextPtr()->IASetInputLayout(om.GetBillboardInputLayoutPtr());

	// プリミティブ形状のセット
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ブレンディングのセット
	if(!blend){
		gm.SetBlendState(BLEND_STATE::BLEND_ALIGNMENT);
	} else if(blend > 0){
		gm.SetBlendState(BLEND_STATE::BLEND_ADD);
	} else{
		gm.SetBlendState(BLEND_STATE::BLEND_SUBTRACT);
	}

	// ラスタライザステート
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// デプスステンシルステート
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	// シェーダのセット
	gm.GetContextPtr()->VSSetShader(om.GetBillboardVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	if(pTexView){
		gm.GetContextPtr()->PSSetShader(om.GetPixelShederTexturePtr(), NULL, 0);
	} else{
		gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
	}

	//ポリゴン描画
	gm.GetContextPtr()->Draw(4, 0);
	gm.SetBlendState(BLEND_STATE::BLEND_ALIGNMENT);
}

inline void Billboard::UpdateMatrix(Camera* pCamera)
{
	XMMATRIX matWorld, matScale, matRot, matViewInv;

	matWorld = XMMatrixIdentity();

	XMMATRIX matView;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matView.r[i].m128_f32[j] = pCamera->GetViewMatrix().r[i][j];
		}
	}

	matViewInv = XMMatrixTranspose(matView);
	matViewInv.r[0].m128_f32[3] = 0.0f;
	matViewInv.r[1].m128_f32[3] = 0.0f;
	matViewInv.r[2].m128_f32[3] = 0.0f;

	if(bYBillboard_){
		matViewInv.r[0].m128_f32[1] = 0.0f;
		matViewInv.r[1].m128_f32[0] = 0.0f;
		matViewInv.r[1].m128_f32[1] = 1.0f;
		matViewInv.r[1].m128_f32[2] = 0.0f;
		matViewInv.r[2].m128_f32[1] = 0.0f;
	}

	// 拡大縮小処理
	matScale = XMMatrixScaling(scale_.x, scale_.y, 0.0f);

	// 回転の正規化
	rotation_.x = WrapRadian(rotation_.x);
	rotation_.y = WrapRadian(rotation_.y);
	rotation_.z = WrapRadian(rotation_.z);

	// 回転処理
	matRot = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, rotation_.z);

	// 行列の合成
	matWorld = XMMatrixMultiply(matWorld, matScale);
	matWorld = XMMatrixMultiply(matWorld, matRot);
	matWorld = matWorld * matViewInv;

	// 平行移動
	matWorld.r[3].m128_f32[0] = position_.x;
	matWorld.r[3].m128_f32[1] = position_.y;
	matWorld.r[3].m128_f32[2] = position_.z;

	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			world_.r[i][j] = matWorld.r[i].m128_f32[j];
		}
	}
}
