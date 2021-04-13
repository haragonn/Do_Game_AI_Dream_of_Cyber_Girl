/*==============================================================================
	[Sprite.cpp]
														Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../../h/2D/Sprite.h"
#include "../../h/Framework/GraphicManager.h"
#include "../../h/2D/SpriteManager.h"
#include "../../h/2D/SpriteInstancing.h"
#include "../../h/Texture/Texture.h"
#include "../../h/Utility/ideaType.h"
#include "../../h/Utility/ConstBufferType.h"
#include "../../h/Utility/ideaMath.h"

//------------------------------------------------------------------------------
// using namespace
//------------------------------------------------------------------------------
using namespace std;
using namespace DirectX;
using namespace idea;

namespace{
	//------------------------------------------------------------------------------
	// ビューポートの設定
	//------------------------------------------------------------------------------
	void SetViewPort()
	{
		GraphicManager& gm = GraphicManager::Instance();

		// ビューポートの設定
		D3D11_VIEWPORT viewPort;
		viewPort.TopLeftX = 0;
		viewPort.TopLeftY = 0;
		viewPort.Width = (FLOAT)gm.GetWidth();
		viewPort.Height = (FLOAT)gm.GetHeight();
		viewPort.MinDepth = 0.0f;
		viewPort.MaxDepth = 1.0f;

		gm.GetContextPtr()->RSSetViewports(1, &viewPort);
	}
}

//------------------------------------------------------------------------------
// コンストラクタ
//------------------------------------------------------------------------------
Sprite::Sprite(){}

//------------------------------------------------------------------------------
// デストラクタ
//------------------------------------------------------------------------------
Sprite::~Sprite()
{
	// SpriteInstancingManagerの登録解除
	if(vecSpriteInstancingPtr_.size() > 0){
		for(auto it = begin(vecSpriteInstancingPtr_), itEnd = end(vecSpriteInstancingPtr_); it != itEnd; ++it){
			if(*it){
				(*it)->RemoveSprite(*this);
			}
		}
	}
}

//------------------------------------------------------------------------------
// 矩形の描画
//------------------------------------------------------------------------------
void Sprite::DrawRect(int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	SpriteManager& sm = SpriteManager::Instance();

	// 画面外なら終了
	float halfWidth = size_.x * 0.5f;
	float halfHeight = size_.y * 0.5f;
	float longVicinity = max(halfWidth, halfHeight) * ideaMath::ROOT2;

	if(position_.x + longVicinity < 0.0f
		|| position_.x - longVicinity > gm.GetWidth()
		|| position_.y + longVicinity < 0.0f
		|| position_.y - longVicinity > gm.GetHeight()){ return; }

	// 頂点バッファ書き込み
	D3D11_MAPPED_SUBRESOURCE msr;
	ID3D11Buffer* pVBuf = sm.GetRectVertexBufferPtr();

	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VertexData2D* vd = (VertexData2D*)msr.pData;

	// 頂点情報の初期化
	vd[0] = { Vector3D(0.0f, 0.0f, 0.0f), color_, Vector2D(0.0f, 0.0f) };
	vd[1] = { Vector3D(0.0f, 0.0f, 0.0f), color_, Vector2D(1.0f, 0.0f) };
	vd[2] = { Vector3D(0.0f, 0.0f, 0.0f), color_, Vector2D(0.0f, 1.0f) };
	vd[3] = { Vector3D(0.0f, 0.0f, 0.0f), color_, Vector2D(1.0f, 1.0f) };

	// 頂点情報の計算
	float x, y, axisX, axisY;

	float tSin = sinf(radian_);
	float tCos = cosf(radian_);

	for(int i = SpriteManager::RECT_VERTEX_NUM - 1; i >= 0; --i){
		x = (i % 2) ? halfWidth : -halfWidth;
		y = (i > 1) ? halfHeight : -halfHeight;

		axisX = x * tCos - y * tSin;
		axisY = x * tSin + y * tCos;

		vd[i].pos.x = axisX + position_.x;
		vd[i].pos.y = axisY + position_.y;
	}

	// 頂点バッファ書き込み終了
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// 頂点バッファのセット
	UINT stride = sizeof(VertexData2D);
	UINT offset = 0;

	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// 入力レイアウトのセット
	gm.GetContextPtr()->IASetInputLayout(sm.GetInputLayoutPtr());

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

	// シェーダのセット
	gm.GetContextPtr()->VSSetShader(sm.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->PSSetShader(sm.GetPixelShederDefaultPtr(), NULL, 0);
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// ビューポートの設定
	SetViewPort();

	//ポリゴン描画
	gm.GetContextPtr()->Draw(SpriteManager::RECT_VERTEX_NUM, 0);
}

//------------------------------------------------------------------------------
// 円の描画
//------------------------------------------------------------------------------
void Sprite::DrawCircle(float ratio, int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	SpriteManager& sm = SpriteManager::Instance();

	// 画面外なら終了
	float halfWidth = size_.x * 0.5f;
	float halfHeight = size_.y * 0.5f;
	float longVicinity = max(halfWidth, halfHeight) * ideaMath::ROOT2;

	if(position_.x + longVicinity < 0.0f
		|| position_.x - longVicinity > gm.GetWidth()
		|| position_.y + longVicinity < 0.0f
		|| position_.y - longVicinity > gm.GetHeight()){ return; }

	// 1.0f以上なら余剰を取る
	if(ratio > 1.0f){ ratio = ratio - (int)ratio; }
	// 0.0f以下なら終了
	if(ratio <= 0.0f){ return; }

	// 頂点バッファ書き込み
	D3D11_MAPPED_SUBRESOURCE msr;
	ID3D11Buffer* pVBuf = sm.GetCircleVertexBufferPtr();

	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VertexData2D* vd = (VertexData2D*)msr.pData;

	// 頂点情報の計算
	vd[0].pos.x = position_.x;
	vd[0].pos.y = position_.y;
	vd[0].color = color_;
	vd[0].tex.x = 0.0f;
	vd[0].tex.y = 0.0f;

	for(int i = 1; i < SpriteManager::CIRCLE_VERTEX_NUM; ++i){
		vd[i].pos.x = position_.x + max(halfWidth, halfHeight) * -sinf(ideaMath::PI * 2.0f - radian_ - ideaMath::PI * 2.0f / (SpriteManager::CIRCLE_VERTEX_NUM - 2) * (i - 1));
		vd[i].pos.y = position_.y + max(halfWidth, halfHeight) * -cosf(ideaMath::PI * 2.0f - radian_ - ideaMath::PI * 2.0f / (SpriteManager::CIRCLE_VERTEX_NUM - 2) * (i - 1));
		vd[i].color = color_;
		vd[1].tex.x = 0.0f;
		vd[1].tex.y = 0.0f;
	}

	// 頂点バッファ書き込み終了
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// 頂点バッファのセット
	UINT stride = sizeof(VertexData2D);
	UINT offset = 0;

	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// インデクスバッファのセット
	gm.GetContextPtr()->IASetIndexBuffer(sm.GetCircleIndexBufferPtr(), DXGI_FORMAT_R16_UINT, 0);

	// 入力レイアウトのセット
	gm.GetContextPtr()->IASetInputLayout(sm.GetInputLayoutPtr());

	// プリミティブ形状のセット
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ブレンディングのセット
	if(!blend){
		gm.SetBlendState(BLEND_STATE::BLEND_ALIGNMENT);
	} else if(blend > 0){
		gm.SetBlendState(BLEND_STATE::BLEND_ADD);
	} else{
		gm.SetBlendState(BLEND_STATE::BLEND_SUBTRACT);
	}

	// シェーダのセット
	gm.GetContextPtr()->VSSetShader(sm.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->PSSetShader(sm.GetPixelShederDefaultPtr(), NULL, 0);
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// ビューポートの設定
	SetViewPort();

	//ポリゴン描画
	gm.GetContextPtr()->DrawIndexed((int)((SpriteManager::CIRCLE_VERTEX_NUM - 2) * ratio) * 3, 0, 0);
}

//------------------------------------------------------------------------------
// 光エフェクトの描画
//------------------------------------------------------------------------------
void Sprite::DrawPhoton(float ratio, int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	SpriteManager& sm = SpriteManager::Instance();

	// 画面外なら終了
	float halfWidth = size_.x * 0.5f;
	float halfHeight = size_.y * 0.5f;
	float longVicinity = max(halfWidth, halfHeight) * ideaMath::ROOT2;

	if(position_.x + longVicinity < 0.0f
		|| position_.x - longVicinity > gm.GetWidth()
		|| position_.y + longVicinity < 0.0f
		|| position_.y - longVicinity > gm.GetHeight()){
		return;
	}

	// 1.0f以上なら余剰を取る
	if(ratio > 1.0f){ ratio = ratio - (int)ratio; }
	// 0.0f以下なら終了
	if(ratio <= 0.0f){ return; }

	// 頂点バッファ書き込み
	D3D11_MAPPED_SUBRESOURCE msr;
	ID3D11Buffer* pVBuf = sm.GetCircleVertexBufferPtr();

	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VertexData2D* vd = (VertexData2D*)msr.pData;

	// 頂点情報の計算
	vd[0].pos.x = position_.x;
	vd[0].pos.y = position_.y;
	vd[0].color = color_;
	vd[0].tex.x = 0.0f;
	vd[0].tex.y = 0.0f;

	for(int i = 1; i < SpriteManager::CIRCLE_VERTEX_NUM; ++i){
		vd[i].pos.x = position_.x + max(halfWidth, halfHeight) * -sinf(ideaMath::PI * 2.0f - radian_ - ideaMath::PI * 2.0f / (SpriteManager::CIRCLE_VERTEX_NUM - 2) * (i - 1));
		vd[i].pos.y = position_.y + max(halfWidth, halfHeight) * -cosf(ideaMath::PI * 2.0f - radian_ - ideaMath::PI * 2.0f / (SpriteManager::CIRCLE_VERTEX_NUM - 2) * (i - 1));
		vd[i].color = color_;
		vd[i].color.a = 0.0f;
		vd[1].tex.x = 0.0f;
		vd[1].tex.y = 0.0f;
	}

	// 頂点バッファ書き込み終了
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// 頂点バッファのセット
	UINT stride = sizeof(VertexData2D);
	UINT offset = 0;

	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// インデックスバッファのセット
	gm.GetContextPtr()->IASetIndexBuffer(sm.GetCircleIndexBufferPtr(), DXGI_FORMAT_R16_UINT, 0);

	// 入力レイアウトのセット
	gm.GetContextPtr()->IASetInputLayout(sm.GetInputLayoutPtr());

	// プリミティブ形状のセット
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ブレンディングのセット
	if(!blend){
		gm.SetBlendState(BLEND_STATE::BLEND_ALIGNMENT);
	} else if(blend > 0){
		gm.SetBlendState(BLEND_STATE::BLEND_ADD);
	} else{
		gm.SetBlendState(BLEND_STATE::BLEND_SUBTRACT);
	}

	// シェーダのセット
	gm.GetContextPtr()->VSSetShader(sm.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->PSSetShader(sm.GetPixelShederDefaultPtr(), NULL, 0);
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// ビューポートの設定
	SetViewPort();

	//ポリゴン描画
	gm.GetContextPtr()->DrawIndexed((int)((SpriteManager::CIRCLE_VERTEX_NUM - 2) * ratio) * 3, 0, 0);
}

//------------------------------------------------------------------------------
// テクスチャの描画
//------------------------------------------------------------------------------
void Sprite::DrawTexture(const Texture& tex, int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	SpriteManager& sm = SpriteManager::Instance();

	// 画面外なら終了
	float halfWidth = size_.x * 0.5f;
	float halfHeight = size_.y * 0.5f;
	float longVicinity = max(halfWidth, halfHeight) * ideaMath::ROOT2;

	if(position_.x + longVicinity < 0.0f
		|| position_.x - longVicinity > gm.GetWidth()
		|| position_.y + longVicinity < 0.0f
		|| position_.y - longVicinity > gm.GetHeight()){ return; }

	// 頂点バッファ書き込み
	D3D11_MAPPED_SUBRESOURCE msr;
	ID3D11Buffer* pVBuf = sm.GetRectVertexBufferPtr();

	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VertexData2D* vd = (VertexData2D*)msr.pData;

	vd[0] = { Vector3D(0.0f, 0.0f, 0.0f), color_, Vector2D(0.0f, 0.0f) };
	vd[1] = { Vector3D(0.0f, 0.0f, 0.0f), color_, Vector2D(1.0f, 0.0f) };
	vd[2] = { Vector3D(0.0f, 0.0f, 0.0f), color_, Vector2D(0.0f, 1.0f) };
	vd[3] = { Vector3D(0.0f, 0.0f, 0.0f), color_, Vector2D(1.0f, 1.0f) };

	// 頂点情報の計算
	float x, y, axisX, axisY;

	float tSin = sinf(radian_);
	float tCos = cosf(radian_);

	for(int i = SpriteManager::RECT_VERTEX_NUM - 1; i >= 0; --i){
		x = (i % 2) ? halfWidth : -halfWidth;
		y = (i > 1) ? halfHeight : -halfHeight;

		axisX = x * tCos - y * tSin;
		axisY = x * tSin + y * tCos;

		vd[i].pos.x = axisX + position_.x;
		vd[i].pos.y = axisY + position_.y;
		vd[i].tex.x = (!!(i % 2) ^ bReversedU_) ? 1.0f : 0.0f;
		vd[i].tex.y = (!!(i > 1) ^ bReversedV_) ? 1.0f : 0.0f;
	}

	// 頂点バッファ書き込み終了
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// 頂点バッファのセット
	UINT stride = sizeof(VertexData2D);
	UINT offset = 0;

	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// テクスチャ書き込み
	ID3D11ShaderResourceView* pTexView = tex.GetTextureViewPtr();

	if(pTexView){
		gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
	}

	// 入力レイアウトのセット
	gm.GetContextPtr()->IASetInputLayout(sm.GetInputLayoutPtr());

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

	// シェーダのセット
	gm.GetContextPtr()->VSSetShader(sm.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	if(pTexView){
		gm.GetContextPtr()->PSSetShader(sm.GetPixelShederTexturePtr(), NULL, 0);
	} else{
		gm.GetContextPtr()->PSSetShader(sm.GetPixelShederDefaultPtr(), NULL, 0);
	}
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// ビューポートの設定
	SetViewPort();

	//ポリゴン描画
	gm.GetContextPtr()->Draw(SpriteManager::RECT_VERTEX_NUM, 0);
}

//------------------------------------------------------------------------------
// 等分割されたテクスチャの描画
//------------------------------------------------------------------------------
void Sprite::DrawDividedTexture(const Texture& tex, int uNum, int vNum, int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	SpriteManager& sm = SpriteManager::Instance();

	// 画面外なら終了
	float halfWidth = size_.x * 0.5f;
	float halfHeight = size_.y * 0.5f;
	float longVicinity = max(halfWidth, halfHeight) * ideaMath::ROOT2;

	if(position_.x + longVicinity < 0.0f
		|| position_.x - longVicinity > gm.GetWidth()
		|| position_.y + longVicinity < 0.0f
		|| position_.y - longVicinity > gm.GetHeight()){
		return;
	}

	// 頂点バッファ書き込み
	D3D11_MAPPED_SUBRESOURCE msr;
	ID3D11Buffer* pVBuf = sm.GetRectVertexBufferPtr();

	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VertexData2D* vd = (VertexData2D*)msr.pData;

	// 頂点情報の計算
	float x, y, axisX, axisY;

	float tSin = sinf(radian_);
	float tCos = cosf(radian_);

	float u1 = tex.GetDivU() * uNum;
	float u2 = tex.GetDivU() * (uNum + 1);
	float v1 = tex.GetDivV() * vNum;
	float v2 = tex.GetDivV() * (vNum + 1);

	for(int i = SpriteManager::RECT_VERTEX_NUM - 1; i >= 0; --i){
		x = (i % 2) ? halfWidth : -halfWidth;
		y = (i > 1) ? halfHeight : -halfHeight;

		axisX = x * tCos - y * tSin;
		axisY = x * tSin + y * tCos;

		vd[i].pos.x = axisX + position_.x;
		vd[i].pos.y = axisY + position_.y;
		vd[i].color = color_;
		vd[i].tex.x = (!!(i % 2) ^ bReversedU_) ? u2 : u1;
		vd[i].tex.y = (!!(i > 1) ^ bReversedV_) ? v2 : v1;
	}

	// 頂点バッファ書き込み終了
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// 頂点バッファのセット
	UINT stride = sizeof(VertexData2D);
	UINT offset = 0;

	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// テクスチャ書き込み
	ID3D11ShaderResourceView* pTexView = tex.GetTextureViewPtr();

	if(pTexView){
		gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
	}

	// 入力レイアウトのセット
	gm.GetContextPtr()->IASetInputLayout(sm.GetInputLayoutPtr());

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

	// シェーダのセット
	gm.GetContextPtr()->VSSetShader(sm.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	if(pTexView){
		gm.GetContextPtr()->PSSetShader(sm.GetPixelShederTexturePtr(), NULL, 0);
	} else{
		gm.GetContextPtr()->PSSetShader(sm.GetPixelShederDefaultPtr(), NULL, 0);
	}
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// ビューポートの設定
	SetViewPort();

	//ポリゴン描画
	gm.GetContextPtr()->Draw(SpriteManager::RECT_VERTEX_NUM, 0);
}

//------------------------------------------------------------------------------
// 表示範囲を指定したテクスチャーの描画
//------------------------------------------------------------------------------
void Sprite::DrawDelimitedTexture(const Texture& tex, const float& u, const float& v, const float& width, const float& height, int blend)
{
	GraphicManager& gm = GraphicManager::Instance();
	SpriteManager& sm = SpriteManager::Instance();

	// 画面外なら終了
	float halfWidth = size_.x * 0.5f;
	float halfHeight = size_.y * 0.5f;
	float longVicinity = max(halfWidth, halfHeight) * ideaMath::ROOT2;

	if(position_.x + longVicinity < 0.0f
		|| position_.x - longVicinity > gm.GetWidth()
		|| position_.y + longVicinity < 0.0f
		|| position_.y - longVicinity > gm.GetHeight()){
		return;
	}

	// 頂点バッファ書き込み
	D3D11_MAPPED_SUBRESOURCE msr;
	ID3D11Buffer* pVBuf = sm.GetRectVertexBufferPtr();

	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VertexData2D* vd = (VertexData2D*)msr.pData;

	// 頂点情報の計算
	float x, y, axisX, axisY;

	float tSin = sinf(radian_);
	float tCos = cosf(radian_);

	float u1 = u;
	float u2 = u + width;
	float v1 = v;
	float v2 = v + height;

	for(int i = SpriteManager::RECT_VERTEX_NUM - 1; i >= 0; --i){
		x = (i % 2) ? halfWidth : -halfWidth;
		y = (i > 1) ? halfHeight : -halfHeight;

		axisX = x * tCos - y * tSin;
		axisY = x * tSin + y * tCos;

		vd[i].pos.x = axisX + position_.x;
		vd[i].pos.y = axisY + position_.y;
		vd[i].color = color_;
		vd[i].tex.x = (!!(i % 2) ^ bReversedU_) ? u2 : u1;
		vd[i].tex.y = (!!(i > 1) ^ bReversedV_) ? v2 : v1;
	}

	// 頂点バッファ書き込み終了
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// 頂点バッファのセット
	UINT stride = sizeof(VertexData2D);
	UINT offset = 0;

	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// テクスチャ書き込み
	ID3D11ShaderResourceView* pTexView = tex.GetTextureViewPtr();

	if(pTexView){
		gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
	}

	// 入力レイアウトのセット
	gm.GetContextPtr()->IASetInputLayout(sm.GetInputLayoutPtr());

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

	// シェーダのセット
	gm.GetContextPtr()->VSSetShader(sm.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	if(pTexView){
		gm.GetContextPtr()->PSSetShader(sm.GetPixelShederTexturePtr(), NULL, 0);
	} else{
		gm.GetContextPtr()->PSSetShader(sm.GetPixelShederDefaultPtr(), NULL, 0);
	}
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// ビューポートの設定
	SetViewPort();

	//ポリゴン描画
	gm.GetContextPtr()->Draw(SpriteManager::RECT_VERTEX_NUM, 0);
}