/*==============================================================================
[Sphere.cpp]
Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// インクルードファイル
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

		//定数バッファ
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

		// 定数バッファ内容更新
		gm.GetContextPtr()->UpdateSubresource(om.GetConstBufferPtr(), 0, NULL, &cbuff, 0, 0);

		// 定数バッファ
		UINT cb_slot = 1;
		ID3D11Buffer* cb[] = { om.GetConstBufferPtr() };
		gm.GetContextPtr()->VSSetConstantBuffers(cb_slot, 1, cb);
	}

	void SetShadowConstBuffer(Matrix4x4& world, Camera* pCamera, Color& color, ShadowCamera* pScmr)
	{
		GraphicManager& gm = GraphicManager::Instance();
		ObjectManager& om = ObjectManager::Instance();

		//定数バッファ
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

		// 定数バッファ内容更新
		gm.GetContextPtr()->UpdateSubresource(om.GetShadowConstBufferPtr(), 0, NULL, &cbuff, 0, 0);

		// 定数バッファ
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

	//定数バッファ
	SetConstBuffer(world_, pCamera, color_);

	// バッファ書き込み
	ID3D11Buffer* pVBuf = om.GetSphereVertexBufferPtr();

	// 頂点バッファのセット
	UINT stride = sizeof(MeshVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// インデックスバッファのセット
	gm.GetContextPtr()->IASetIndexBuffer(om.GetSphereIndexBufferPtr(), DXGI_FORMAT_R16_UINT, 0);

	// 入力レイアウトのセット
	gm.GetContextPtr()->IASetInputLayout(om.GetInputLayoutPtr());

	// プリミティブ形状のセット
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ラスタライザステート
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// デプスステンシルステート
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	// シェーダのセット
	gm.GetContextPtr()->VSSetShader(om.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	//ポリゴン描画
	gm.GetContextPtr()->DrawIndexed(om.GetSphereIndexSize(), 0, 0);
}

inline void Sphere::DrawShadowSphere(Camera* pCamera, int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();

	//定数バッファ
	SetShadowConstBuffer(world_, pCamera, color_, pScmr_);

	// バッファ書き込み
	ID3D11Buffer* pVBuf = om.GetSphereVertexBufferPtr();

	// 頂点バッファのセット
	UINT stride = sizeof(MeshVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// インデックスバッファのセット
	gm.GetContextPtr()->IASetIndexBuffer(om.GetSphereIndexBufferPtr(), DXGI_FORMAT_R16_UINT, 0);

	// 入力レイアウトのセット
	gm.GetContextPtr()->IASetInputLayout(om.GetShadowInputLayoutPtr());

	// プリミティブ形状のセット
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ラスタライザステート
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// デプスステンシルステート
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	ID3D11ShaderResourceView* const pSRV[1] = { NULL };
	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);

	// テクスチャ書き込み
	ID3D11ShaderResourceView* pTexView = gm.GetShaderResourceViewPtrArray()[1];

	if(pTexView){
		gm.GetContextPtr()->PSSetShaderResources(1, 1, &pTexView);
	}

	// シェーダのセット
	gm.GetContextPtr()->VSSetShader(om.GetShadowVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->PSSetShader(om.GetPixelShaderShadowPtr(), NULL, 0);
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	//ポリゴン描画
	gm.GetContextPtr()->DrawIndexed(om.GetSphereIndexSize(), 0, 0);

	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);
}

inline void Sphere::DrawTextureSphere(Camera* pCamera, const Texture& tex, int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();

	//定数バッファ
	SetConstBuffer(world_, pCamera, color_);

	// バッファ書き込み
	ID3D11Buffer* pVBuf = om.GetSphereVertexBufferPtr();

	// 頂点バッファのセット
	UINT stride = sizeof(MeshVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// インデックスバッファのセット
	gm.GetContextPtr()->IASetIndexBuffer(om.GetSphereIndexBufferPtr(), DXGI_FORMAT_R16_UINT, 0);

	// テクスチャ書き込み
	ID3D11ShaderResourceView* pTexView = tex.GetTextureViewPtr();
	if(pTexView){
		gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
	}

	// 入力レイアウトのセット
	gm.GetContextPtr()->IASetInputLayout(om.GetInputLayoutPtr());

	// プリミティブ形状のセット
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ラスタライザステート
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// デプスステンシルステート
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	// シェーダのセット
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

	//ポリゴン描画
	gm.GetContextPtr()->DrawIndexed(om.GetSphereIndexSize(), 0, 0);
}

inline void Sphere::DrawTextureShadowSphere(Camera* pCamera, const Texture& tex, int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();

	//定数バッファ
	SetShadowConstBuffer(world_, pCamera, color_, pScmr_);

	// バッファ書き込み
	ID3D11Buffer* pVBuf = om.GetSphereVertexBufferPtr();

	// 頂点バッファのセット
	UINT stride = sizeof(MeshVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// インデックスバッファのセット
	gm.GetContextPtr()->IASetIndexBuffer(om.GetSphereIndexBufferPtr(), DXGI_FORMAT_R16_UINT, 0);

	// 入力レイアウトのセット
	gm.GetContextPtr()->IASetInputLayout(om.GetShadowInputLayoutPtr());

	// プリミティブ形状のセット
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ラスタライザステート
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// デプスステンシルステート
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	ID3D11ShaderResourceView* const pSRV[1] = { NULL };
	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);

	// テクスチャ書き込み
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

	// シェーダのセット
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

	//ポリゴン描画
	gm.GetContextPtr()->DrawIndexed(om.GetSphereIndexSize(), 0, 0);

	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);
}
