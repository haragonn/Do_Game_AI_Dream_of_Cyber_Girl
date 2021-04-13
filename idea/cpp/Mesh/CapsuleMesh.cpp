#include "../../h/Mesh/CapsuleMesh.h"
#include "../../h/Framework/GraphicManager.h"
#include "../../h/3D/ObjectManager.h"
#include "../../h/3D/3DStruct.h"
#include "../../h/3D/Collision3D.h"
#include "../../h/Utility/ConstBufferType.h"
#include "../../h/Environment/Camera.h"
#include "../../h/Environment/ShadowCamera.h"
#include "../../h/Utility/ideaMath.h"
#include "../../h/Utility/ideaColor.h"
#include "../../h/Utility/ideaUtility.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

using namespace idea;
using namespace std;
using namespace DirectX;

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

CapsuleMesh::CapsuleMesh() :
	radius_(0.0f),
	height_(0.0f),
	pVertexBuffer_(nullptr),
	pIndexBuffer_(nullptr),
	indexSize_(0),
	pScmr_(nullptr){}

bool CapsuleMesh::Create(float diameter, float height)
{
	GraphicManager& gm = GraphicManager::Instance();
	if(!gm.GetContextPtr()
		|| pVertexBuffer_){ return false; }

	HRESULT hr = FALSE;

	height_ = height;

	int verticalSize = 16;
	int horizontalSize = 16 * 2;

	// 頂点バッファ
	{
		vector<MeshVertexData> vecVertex;
		radius_ = diameter * 0.5f;

		for(int i = 0; i <= verticalSize; ++i)
		{
			float v = 1 - (float)i / verticalSize;

			float latitude = (i * ideaMath::PI / verticalSize) - ideaMath::PIDIV2;

			float dy, dxz;

			XMScalarSinCos(&dy, &dxz, latitude);

			Vector3D centerPos = Vector3D(0.0f, radius_ - height * 0.5f, 0.0f);
			if(i >= (verticalSize / 2)) {
				centerPos = Vector3D(0.0f, height * 0.5f - radius_, 0.0f);
			}

			float CapsuleTotalFront = diameter * ideaMath::PI / 2.0f;
			float SylinderTotalFront = abs(height - diameter);

			float CapsuleRate = CapsuleTotalFront / (CapsuleTotalFront + SylinderTotalFront) * 0.5f;

			if(v <= 0.5f) {
				v *= CapsuleRate;
			} else {
				v = 1.0f - ((1.0f - v) * CapsuleRate);
			}

			for(int j = 0; j <= horizontalSize; ++j)
			{
				float u = 1.0f - (float)j / horizontalSize;

				float longitude = j * ideaMath::PI2 / horizontalSize;
				float dx, dz;

				XMScalarSinCos(&dx, &dz, longitude);

				dx *= dxz;
				dz *= dxz;

				Vector3D normal{ dx, dy, dz };

				MeshVertexData vertex;
				vertex.pos = normal * radius_ + centerPos;
				vertex.nor = normal;
				vertex.color = ideaColor::WHITE;
				vertex.tex = Vector2D(u, v);

				vecVertex.push_back(vertex);
			}
		}

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(MeshVertexData) * vecVertex.size();
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = &vecVertex[0];
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		hr = gm.GetDevicePtr()->CreateBuffer(&bd, &InitData, &pVertexBuffer_);
		if(FAILED(hr)){ return false; }
	}

	// インデックスバッファ
	{
		vector<WORD> vecIndex;
		int stride = horizontalSize + 1;

		for(int i = 0; i < verticalSize; ++i){
			for(int j = 0; j <= horizontalSize; ++j){
				size_t nextI = i + 1;
				size_t nextJ = (j + 1) % stride;

				vecIndex.push_back((WORD)(i * stride + nextJ));
				vecIndex.push_back((WORD)(nextI * stride + j));
				vecIndex.push_back((WORD)(i * stride + j));

				vecIndex.push_back((WORD)(nextI * stride + nextJ));
				vecIndex.push_back((WORD)(nextI * stride + j));
				vecIndex.push_back((WORD)(i * stride + nextJ));
			}
		}

		indexSize_ = vecIndex.size();

		D3D11_BUFFER_DESC bd;
		bd.ByteWidth = sizeof(WORD) * indexSize_;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = &vecIndex[0];
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		hr = gm.GetDevicePtr()->CreateBuffer(&bd, &InitData, &pIndexBuffer_);
		if(FAILED(hr)){ return false; }
	}

	return true;
}

void CapsuleMesh::Release()
{
	SafeRelease(pIndexBuffer_);
	SafeRelease(pVertexBuffer_);

	indexSize_ = 0;
}

CapsuleCollider idea::CapsuleMesh::GetCollider()
{
	Vector3D scale = ScalingFromMatrix4x4(world_);

	cc_.SetCapsule(world_, radius_ * idea::Max(scale.x, scale.z), height_ * scale.y, prePosition_);

	return cc_;
}

BaseCollider* idea::CapsuleMesh::GetColliderPtr()
{
	Vector3D scale = ScalingFromMatrix4x4(world_);

	cc_.SetCapsule(world_, radius_ * idea::Max(scale.x, scale.z), height_ * scale.y, prePosition_);

	return &cc_;
}

void CapsuleMesh::SetTexture(Texture& tex)
{
	TextureHolder::SetTexture(&tex);
}

void CapsuleMesh::ExclusionTexture()
{
	TextureHolder::ExclusionTexture();
}

void CapsuleMesh::SetShadow(ShadowCamera& scmr)
{
	pScmr_ = &scmr;
}

void CapsuleMesh::Draw(Camera* pCamera)
{
	if(!pVertexBuffer_){ return; }

	if(pScmr_ && !pScmr_->GetShadowDrawFlag()){
		if(!pTexture_){
			DrawShadowCapsule(pCamera);
		} else{
			DrawTextureShadowCapsule(pCamera, *pTexture_);
		}
	} else{
		if(!pTexture_){
			DrawCapsule(pCamera);
		} else{
			DrawTextureCapsule(pCamera, *pTexture_);
		}
	}
}

inline void CapsuleMesh::DrawCapsule(Camera* pCamera, int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();

	//定数バッファ
	SetConstBuffer(world_, pCamera, color_);

	// バッファ書き込み
	ID3D11Buffer* pVBuf = pVertexBuffer_;

	// 頂点バッファのセット
	UINT stride = sizeof(MeshVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// インデックスバッファのセット
	gm.GetContextPtr()->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R16_UINT, 0);

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
	gm.GetContextPtr()->DrawIndexed(indexSize_, 0, 0);
}

inline void CapsuleMesh::DrawShadowCapsule(Camera* pCamera, int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();

	//定数バッファ
	SetShadowConstBuffer(world_, pCamera, color_, pScmr_);

	// バッファ書き込み
	ID3D11Buffer* pVBuf = pVertexBuffer_;

	// 頂点バッファのセット
	UINT stride = sizeof(MeshVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// インデックスバッファのセット
	gm.GetContextPtr()->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R16_UINT, 0);

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
	gm.GetContextPtr()->DrawIndexed(indexSize_, 0, 0);

	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);
}

inline void CapsuleMesh::DrawTextureCapsule(Camera* pCamera, const Texture& tex, int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();

	//定数バッファ
	SetConstBuffer(world_, pCamera, color_);

	// バッファ書き込み
	ID3D11Buffer* pVBuf = pVertexBuffer_;

	// 頂点バッファのセット
	UINT stride = sizeof(MeshVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// インデックスバッファのセット
	gm.GetContextPtr()->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R16_UINT, 0);

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
	gm.GetContextPtr()->DrawIndexed(indexSize_, 0, 0);
}

inline void CapsuleMesh::DrawTextureShadowCapsule(Camera* pCamera, const Texture& tex, int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();

	//定数バッファ
	SetShadowConstBuffer(world_, pCamera, color_, pScmr_);

	// バッファ書き込み
	ID3D11Buffer* pVBuf = pVertexBuffer_;

	// 頂点バッファのセット
	UINT stride = sizeof(MeshVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// インデックスバッファのセット
	gm.GetContextPtr()->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R16_UINT, 0);

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
	gm.GetContextPtr()->DrawIndexed(indexSize_, 0, 0);

	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);
}