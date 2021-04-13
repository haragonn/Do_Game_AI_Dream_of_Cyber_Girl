#include "../../h/Environment/ShadowCamera.h"
#include "../../../idea/h/Framework/GraphicManager.h"
#include "../../h/3D/ObjectManager.h"
#include "../../h/3D/Object.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <directxmath.h>

using namespace DirectX;
using namespace idea;

void ShadowCamera::Init(float range, float nearZ, float farZ)
{
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();
	if(!gm.GetContextPtr()){ return; }

	nearZ_ = nearZ;
	farZ_ = farZ;

	XMFLOAT4X4 matProj;
	XMStoreFloat4x4(&matProj, XMMatrixIdentity());
	XMStoreFloat4x4(&matProj, XMMatrixOrthographicLH(5.0f * range, 5.0f * range, nearZ_, farZ_));

	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			proj_.r[i][j] = matProj.m[i][j];
		}
	}

	viewPort_.topLeftX = 0.0f;
	viewPort_.topLeftY = 0.0f;
	viewPort_.width = (float)gm.GetWidth();
	viewPort_.height = (float)gm.GetHeight();
	viewPort_.minDepth = 0.0f;
	viewPort_.maxDepth = 1.0f;
}


void ShadowCamera::SetPosition(const Vector3D& pos)
{
	position_ = pos;
}

void ShadowCamera::SetPosition(float posX, float posY, float posZ)
{
	position_.x = posX;
	position_.y = posY;
	position_.z = posZ;
}

void ShadowCamera::MovePosition(const Vector3D& axis)
{
	position_ += axis;
}

void ShadowCamera::MovePosition(float axisX, float axisY, float axisZ)
{
	position_.x += axisX;
	position_.y += axisY;
	position_.z += axisZ;
}

void ShadowCamera::DrawObject()
{
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();
	if(!gm.GetContextPtr()){ return; }

	Vector3D light = om.GetLight() * -1.0f;
	eye_ = light;
	eye_ = eye_.Normalized() * (farZ_ - nearZ_) * 0.5f + position_;
	focus_ = Vector3D(0.0f, 1.0f, 0.0f) - light + position_;
	up_ = Vector3D(0.0f, 1.0f, 0.0f);
	UpdateViewMatrix();

	gm.GetContextPtr()->ClearDepthStencilView(gm.GetDepthStencilViewPtr(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0x0);

	ID3D11RenderTargetView* pathShadow = gm.GetRenderTargetViewPtr(2);

	gm.GetContextPtr()->OMSetRenderTargets(1, &pathShadow, gm.GetDepthStencilViewPtr());

	// ビューポートの設定
	D3D11_VIEWPORT viewPort = {};
	viewPort.TopLeftX = viewPort_.topLeftX;
	viewPort.TopLeftY = viewPort_.topLeftY;
	viewPort.Width = viewPort_.width;
	viewPort.Height = viewPort_.height;
	viewPort.MinDepth = viewPort_.minDepth;
	viewPort.MaxDepth = viewPort_.maxDepth;
	gm.GetContextPtr()->RSSetViewports(1, &viewPort);

	gm.Draw3D();

	bShadowDraw_ = true;

	for(auto& obj : vecObjPtr_){
		if(obj->GetColor().a != 0.0f){
			obj->Draw(this);
		}
	}

	bShadowDraw_ = false;

	gm.EndMask();

	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = (FLOAT)gm.GetWidth();
	viewPort.Height = (FLOAT)gm.GetHeight();
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	gm.DrawShadow(1, viewPort);

	ID3D11ShaderResourceView* const pSRV[1] = { NULL };
	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);

	ID3D11RenderTargetView* pathMain = gm.GetRenderTargetViewPtr(0);
	gm.GetContextPtr()->ClearDepthStencilView(gm.GetDepthStencilViewPtr(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0x0);
	gm.GetContextPtr()->OMSetRenderTargets(1, &pathMain, gm.GetDepthStencilViewPtr());
	gm.Draw3D();
}