#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <DirectXMath.h>
#include "../../h/Mesh/PlaneMesh.h"
#include "../../h/Framework/GraphicManager.h"
#include "../../h/3D/ObjectManager.h"
#include "../../h/3D/3DStruct.h"
#include "../../h/Utility/ConstBufferType.h"
#include "../../h/Environment/Camera.h"
#include "../../h/Environment/ShadowCamera.h"
#include "../../h/Utility/ideaMath.h"
#include "../../h/Utility/ideaColor.h"
#include "../../h/Utility/ideaUtility.h"

using namespace std;
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

PlaneMesh::PlaneMesh() :
	uNum_(1U),
	vNum_(1U),
	vertexSize_(0),
	pVertexBuffer_(nullptr),
	pScmr_(nullptr){}

bool PlaneMesh::Create(float centerX, float centerZ, float widthX, float widthZ, unsigned int uNum, unsigned int vNum, bool bOnePicture)
{
	GraphicManager& gm = GraphicManager::Instance();
	if(!gm.GetContextPtr()
		|| pVertexBuffer_){ return false; }

	widthX_ = widthX;
	widthZ_ = widthZ;
	uNum_ = max(1, uNum);
	vNum_ = max(1, vNum);

	centerX = widthX_ * uNum_ * 0.5f + centerX;
	centerZ = widthZ_ * vNum_ * 0.5f + centerZ;

	lengthRate_ = Max(widthX_, widthZ_) * Max(uNum_, vNum_);

	HRESULT hr;
	{
		vector<float> vecHeight;
		vecHeight.resize((uNum_ + 1) * (vNum_ + 1));

		int cnt = 0;
		for(int j = 0; j < (int)vNum_ + 1; ++j){
			for(int i = 0; i < (int)uNum_ + 1; ++i){
				vecHeight[cnt] = 0.0f;
				++cnt;
			}
		}

		vertexList_.resize((uNum_ + 1) * (vNum_ + 1));

		cnt = 0;
		for(int j = 0; j < (int)vNum_ + 1; ++j){
			for(int i = 0; i < (int)uNum_ + 1; ++i){
				vertexList_[cnt].x = i * widthX_ - centerX;
				vertexList_[cnt].y = vecHeight[cnt];
				vertexList_[cnt].z = j * widthZ_ - centerZ;
				++cnt;
			}
		}

		vertexSize_ = 6 * uNum_ * vNum_;

		// 頂点バッファ
		vector<MeshVertexData> mvd;
		mvd.resize(vertexSize_);

		for(int z = 0; z < (int)vNum_; ++z)
		{
			for(int x = 0; x < (int)uNum_; ++x)
			{
				int i = (z * uNum_ * 6) + (x * 6);
				int jx = z * (uNum_ + 1) + x;
				int jz = (z + 1) * (uNum_ + 1) + x;

				int state = 0;
				if(vecHeight[jx] > 1 && vecHeight[jx + 1] == 0 && vecHeight[jz] == 0 && vecHeight[jz + 1] == 0)state = 1;
				else if(vecHeight[jx] == 0 && vecHeight[jx + 1] == 0 && vecHeight[jz] == 0 && vecHeight[jz + 1] > 1) state = 2;
				else if(vecHeight[jx] > 1 && vecHeight[jx + 1] > 1 && vecHeight[jz] > 1 && vecHeight[jz + 1] == 0) state = 3;
				else if(vecHeight[jx] == 0 && vecHeight[jx + 1] > 1 && vecHeight[jz] > 1 && vecHeight[jz + 1] > 1)  state = 4;

				//頂点座標の設定(2D座標・右回り)
				switch(state){
				case 0:
					mvd[i + 2].pos = Vector3D(x * widthX_ - centerX, vecHeight[jx], z * widthZ_ - centerZ);
					mvd[i + 1].pos = Vector3D((x + 1) * widthX_ - centerX, vecHeight[jx + 1], z * widthZ_ - centerZ);
					mvd[i + 0].pos = Vector3D(x * widthX_ - centerX, vecHeight[jz], (z + 1) * widthZ_ - centerZ);

					mvd[i + 5].pos = Vector3D((x + 1) * widthX_ - centerX, vecHeight[jz + 1], (z + 1) * widthZ_ - centerZ);
					mvd[i + 4].pos = Vector3D(x * widthX_ - centerX, vecHeight[jz], (z + 1) * widthZ_ - centerZ);
					mvd[i + 3].pos = Vector3D((x + 1) * widthX_ - centerX, vecHeight[jx + 1], z * widthZ_ - centerZ);
					break;
				case 1:
				case 2:
				case 3:
				case 4:
					mvd[i + 2].pos = Vector3D(x * widthX_ - centerX, vecHeight[jz], (z + 1) * widthZ_ - centerZ);
					mvd[i + 1].pos = Vector3D(x * widthX_ - centerX, vecHeight[jx], z * widthZ_ - centerZ);
					mvd[i + 0].pos = Vector3D((x + 1) * widthX_ - centerX, vecHeight[jz + 1], (z + 1) * widthZ_ - centerZ);

					mvd[i + 5].pos = Vector3D((x + 1) * widthX_ - centerX, vecHeight[jx + 1], z * widthZ_ - centerZ);
					mvd[i + 4].pos = Vector3D((x + 1) * widthX_ - centerX, vecHeight[jz + 1], (z + 1) * widthZ_ - centerZ);
					mvd[i + 3].pos = Vector3D(x * widthX_ - centerX, vecHeight[jx], z * widthZ_ - centerZ);
					break;
				}

				mvd[i + 0].color = color_;
				mvd[i + 1].color = color_;
				mvd[i + 2].color = color_;

				mvd[i + 3].color = color_;
				mvd[i + 4].color = color_;
				mvd[i + 5].color = color_;

				if(bOnePicture){		// 一枚絵
					float u1 = (float)x / uNum_;
					float u2 = (float)(x + 1) / uNum_;
					float v1 = 1.0f - (float)z / vNum_;
					float v2 = 1.0f - (float)(z + 1) / vNum_;

					mvd[i + 2].tex = Vector2D(u1, v1);
					mvd[i + 1].tex = mvd[i + 3].tex = Vector2D(u2, v1);
					mvd[i + 0].tex = mvd[i + 4].tex = Vector2D(u1, v2);
					mvd[i + 5].tex = Vector2D(u2, v2);
				} else{	// 分割絵
					mvd[i + 2].tex = Vector2D(0.0f, 1.0f);
					mvd[i + 1].tex = mvd[i + 3].tex = Vector2D(1.0f, 1.0f);
					mvd[i + 0].tex = mvd[i + 4].tex = Vector2D(0.0f, 0.0f);
					mvd[i + 5].tex = Vector2D(1.0f, 0.0f);
				}

				Vector3D AB = mvd[i + 0].pos - mvd[i + 1].pos;
				Vector3D BC = mvd[i + 1].pos - mvd[i + 2].pos;
				Vector3D DC = mvd[i + 3].pos - mvd[i + 0].pos;

				Vector3D normalA, normalB;

				normalA = AB.Cross(BC).Normalized();
				normalB = AB.Cross(BC).Normalized();

				mvd[i + 0].nor = normalA;
				mvd[i + 1].nor = normalA;
				mvd[i + 2].nor = normalA;

				mvd[i + 3].nor = normalB;
				mvd[i + 4].nor = normalB;
				mvd[i + 5].nor = normalB;
			}
		}

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(MeshVertexData) * mvd.size();
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = &mvd[0];
		hr = gm.GetDevicePtr()->CreateBuffer(&bd, &InitData, &pVertexBuffer_);
		if(FAILED(hr)){ return false; }
	}

	return true;
}

void PlaneMesh::Release()
{
	SafeRelease(pVertexBuffer_);
}

void PlaneMesh::SetTexture(Texture& tex)
{
	TextureHolder::SetTexture(&tex);
}

void PlaneMesh::ExclusionTexture()
{
	TextureHolder::ExclusionTexture();
}

void PlaneMesh::SetShadow(ShadowCamera& scmr)
{
	pScmr_ = &scmr;
}

float PlaneMesh::GetHeight(float posX, float posZ)
{
	return GetHeight(Vector3D(posX, 0.0f, posZ));
}

float PlaneMesh::GetHeight(const Vector3D& position)
{
	Vector3D p0, p1, p2, v01, v02, v0p, v12, v1p, v20, v2p, vc0, vc1, vc2, vn;
	float py = 0.0f;

	for(unsigned int z = 0; z < vNum_; ++z){
		for(unsigned int x = 0; x < uNum_; ++x){
			p0 = vertexList_[(uNum_ + 1) * (z + 1) + x];
			p1 = vertexList_[(uNum_ + 1) * z + x];
			p2 = vertexList_[(uNum_ + 1) * (z + 1) + x + 1];

			v01 = p1 - p0;
			v0p = position - p0;
			vc0 = v0p.Cross(v01);

			if(vc0.y >= 0.0f){
				v12 = p2 - p1;
				v1p = position - p1;
				vc1 = v1p.Cross(v12);

				if(vc1.y >= 0.0f){
					v20 = p0 - p2;
					v2p = position - p2;
					vc2 = v2p.Cross(v20);

					if(vc2.y >= 0.0f){
						v02 = p2 - p0;
						vn = v02.Cross(v01);
						py = p0.y - ((position.x - p0.x) * vn.x + (position.z - p0.z) * vn.z) / vn.y;

						break;
					}
				}
			}

			p0 = vertexList_[(uNum_ + 1) * z + x];
			p1 = vertexList_[(uNum_ + 1) * z + x + 1];
			p2 = vertexList_[(uNum_ + 1) * (z + 1) + x + 1];

			v01 = p1 - p0;
			v0p = position - p0;
			vc0 = v0p.Cross(v01);

			if(vc0.y >= 0.0f){
				v12 = p2 - p1;
				v1p = position - p1;
				vc1 = v1p.Cross(v12);

				if(vc1.y >= 0.0f){
					v20 = p0 - p2;
					v2p = position - p2;
					vc2 = v2p.Cross(v20);

					if(vc2.y >= 0.0f){
						v02 = p2 - p0;
						vn = v02.Cross(v01);
						py = p0.y - ((position.x - p0.x) * vn.x + (position.z - p0.z) * vn.z) / vn.y;

						break;
					}
				}
			}
		}
	}

	return py;
}

Vector3D PlaneMesh::GetNormal(float posX, float posZ)
{
	return GetNormal(Vector3D(posX, 0.0f, posZ));
}

Vector3D PlaneMesh::GetNormal(const Vector3D& position)
{
	Vector3D p0, p1, p2, v01, v02, v0p, v12, v1p, v20, v2p, vc0, vc1, vc2, vn;
	float py = 0.0f;

	for(unsigned int z = 0; z < vNum_; ++z){
		for(unsigned int x = 0; x < uNum_; ++x){
			p0 = vertexList_[(uNum_ + 1) * (z + 1) + x];
			p1 = vertexList_[(uNum_ + 1) * z + x];
			p2 = vertexList_[(uNum_ + 1) * (z + 1) + x + 1];

			v01 = p1 - p0;
			v0p = position - p0;
			vc0 = v0p.Cross(v01);

			if(vc0.y >= 0.0f){
				v12 = p2 - p1;
				v1p = position - p1;
				vc1 = v1p.Cross(v12);

				if(vc1.y >= 0.0f){
					v20 = p0 - p2;
					v2p = position - p2;
					vc2 = v2p.Cross(v20);

					if(vc2.y >= 0.0f){
						v02 = p2 - p0;

						return v02.Cross(v01).Normalized();
					}
				}
			}

			p0 = vertexList_[(uNum_ + 1) * z + x];
			p1 = vertexList_[(uNum_ + 1) * z + x + 1];
			p2 = vertexList_[(uNum_ + 1) * (z + 1) + x + 1];

			v01 = p1 - p0;
			v0p = position - p0;
			vc0 = v0p.Cross(v01);

			if(vc0.y >= 0.0f){
				v12 = p2 - p1;
				v1p = position - p1;
				vc1 = v1p.Cross(v12);

				if(vc1.y >= 0.0f){
					v20 = p0 - p2;
					v2p = position - p2;
					vc2 = v2p.Cross(v20);

					if(vc2.y >= 0.0f){
						v02 = p2 - p0;

						return v02.Cross(v01).Normalized();
					}
				}
			}
		}
	}

	return Vector3D();
}

void PlaneMesh::Draw(Camera* pCamera)
{
	if(!pVertexBuffer_){ return; }

	if(pScmr_ && !pScmr_->GetShadowDrawFlag()){
		if(!pTexture_){
			DrawPlainShadow(pCamera);
		} else{
			DrawTexturePlainShadow(pCamera, *pTexture_);
		}
	} else{
		if(!pTexture_){
			DrawPlain(pCamera);
		} else{
			DrawTexturePlain(pCamera, *pTexture_);
		}
	}
}

inline void PlaneMesh::DrawPlain(Camera* pCamera, int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();

	//定数バッファ
	SetConstBuffer(world_, pCamera, color_);

	// 頂点バッファのセット
	UINT stride = sizeof(MeshVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

	// 入力レイアウトのセット
	gm.GetContextPtr()->IASetInputLayout(om.GetInputLayoutPtr());

	// プリミティブ形状のセット
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ラスタライザステート
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// デプスステンシルステート
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	// ブレンディングのセット
	//if(!blend){
	//	gm.SetBlendState(BLEND_ALIGNMENT);
	//} else if(blend > 0){
	//	gm.SetBlendState(BLEND_ADD);
	//} else{
	//	gm.SetBlendState(BLEND_SUBTRACT);
	//}

	// シェーダのセット
	gm.GetContextPtr()->VSSetShader(om.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// ビューポートの設定
	//SetViewPort(pCamera);

	//ポリゴン描画
	gm.GetContextPtr()->Draw(vertexSize_, 0);
}

inline void PlaneMesh::DrawPlainShadow(Camera* pCamera, int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();

	//定数バッファ
	SetShadowConstBuffer(world_, pCamera, color_, pScmr_);

	// 頂点バッファのセット
	UINT stride = sizeof(MeshVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

	// 入力レイアウトのセット
	gm.GetContextPtr()->IASetInputLayout(om.GetShadowInputLayoutPtr());

	// プリミティブ形状のセット
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ラスタライザステート
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// デプスステンシルステート
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	// ブレンディングのセット
	//if(!blend){
	//	gm.SetBlendState(BLEND_ALIGNMENT);
	//} else if(blend > 0){
	//	gm.SetBlendState(BLEND_ADD);
	//} else{
	//	gm.SetBlendState(BLEND_SUBTRACT);
	//}

	ID3D11ShaderResourceView* const pSRV[1] = { NULL };
	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);

	// テクスチャ書き込み
	ID3D11ShaderResourceView* pTexView = gm.GetShaderResourceViewPtrArray()[1];

	gm.GetContextPtr()->PSSetShaderResources(1, 1, &pTexView);

	// シェーダのセット
	gm.GetContextPtr()->VSSetShader(om.GetShadowVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->PSSetShader(om.GetPixelShaderShadowPtr(), NULL, 0);
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// ビューポートの設定
	//SetViewPort(pCamera);

	//ポリゴン描画
	gm.GetContextPtr()->Draw(vertexSize_, 0);

	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);
}

inline void PlaneMesh::DrawTexturePlain(Camera* pCamera, const Texture& tex, int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();

	//定数バッファ
	SetConstBuffer(world_, pCamera, color_);

	// 頂点バッファのセット
	UINT stride = sizeof(MeshVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

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

	// ブレンディングのセット
	//if(!blend){
	//	gm.SetBlendState(BLEND_ALIGNMENT);
	//} else if(blend > 0){
	//	gm.SetBlendState(BLEND_ADD);
	//} else{
	//	gm.SetBlendState(BLEND_SUBTRACT);
	//}

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

	// ビューポートの設定
	//SetViewPort(pCamera);

	//ポリゴン描画
	gm.GetContextPtr()->Draw(vertexSize_, 0);
}

inline void PlaneMesh::DrawTexturePlainShadow(Camera* pCamera, const Texture& tex, int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();

	//定数バッファ
	SetShadowConstBuffer(world_, pCamera, color_, pScmr_);

	// 頂点バッファのセット
	UINT stride = sizeof(MeshVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

	// 入力レイアウトのセット
	gm.GetContextPtr()->IASetInputLayout(om.GetShadowInputLayoutPtr());

	// プリミティブ形状のセット
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ラスタライザステート
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// デプスステンシルステート
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	// ブレンディングのセット
	//if(!blend){
	//	gm.SetBlendState(BLEND_ALIGNMENT);
	//} else if(blend > 0){
	//	gm.SetBlendState(BLEND_ADD);
	//} else{
	//	gm.SetBlendState(BLEND_SUBTRACT);
	//}

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

		gm.GetContextPtr()->PSSetShaderResources(1, 1, &pTexView);
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

	// ビューポートの設定
	//SetViewPort(pCamera);

	//ポリゴン描画
	gm.GetContextPtr()->Draw(vertexSize_, 0);

	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);
}
