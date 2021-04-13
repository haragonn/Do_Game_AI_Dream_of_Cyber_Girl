#include "../../h/Environment/Camera.h"
#include "../../h/Framework/GraphicManager.h"
#include "../../h/3D/Object.h"
#include "../../h/Utility/ideaUtility.h"
#include "../../h/Utility/Debug.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <directxmath.h>

using namespace std;
using namespace DirectX;
using namespace idea;

namespace{
	Matrix4x4 XMFLOAT4X4ToMatrix4x4(const XMFLOAT4X4& xmf)
	{
		Matrix4x4 mat;

		for(int i = 4 - 1; i >= 0; --i){
			for(int j = 4 - 1; j >= 0; --j){
				mat.r[i][j] = xmf.m[i][j];
			}
		}

		return mat;
	}
}

Camera::Camera() :
	nearZ_(0.1f),
	farZ_(100.0f),
	eye_(Vector3D(0.0f, 0.0f, -5.0f)),
	focus_(Vector3D(0.0f, 0.0f, 0.0f)),
	up_(Vector3D(0.0f, 1.0f, 0.0f)),
	radius_(1.0f)
{
	GraphicManager& gm = GraphicManager::Instance();

	viewPort_.topLeftX = 0.0f;
	viewPort_.topLeftY = 0.0f;
	viewPort_.width = (float)gm.GetWidth();
	viewPort_.height = (float)gm.GetHeight();
	viewPort_.minDepth = 0.0f;
	viewPort_.maxDepth = 1.0f;
}

Camera::~Camera()
{
	ResetObject();
}

void Camera::Init(float viewAngle, float aspect, float nearZ, float farZ)
{
	GraphicManager& gm = GraphicManager::Instance();
	if(!gm.GetContextPtr()){ return; }

	viewAngle_ = viewAngle;
	aspect_ = aspect;
	nearZ_ = nearZ;
	farZ_ = farZ;

	XMFLOAT4X4 matProj;
	XMStoreFloat4x4(&matProj, XMMatrixIdentity());
	XMStoreFloat4x4(&matProj, XMMatrixPerspectiveFovLH(viewAngle_, aspect_, nearZ_, (farZ_ > nearZ_) ? farZ_ : nearZ_ + 0.1f));
	proj_ = XMFLOAT4X4ToMatrix4x4(matProj);

	eye_ = Vector3D(0.0f, 0.0f, -5.0f);
	focus_ = Vector3D(0.0f, 0.0f, 0.0f);
	up_ = Vector3D(0.0f, 1.0f, 0.0f);

	viewPort_.topLeftX = 0.0f;
	viewPort_.topLeftY = 0.0f;
	viewPort_.width = (float)gm.GetWidth();
	viewPort_.height = (float)gm.GetHeight();
	viewPort_.minDepth = 0.0f;
	viewPort_.maxDepth = 1.0f;

	UpdateViewMatrix();
}

void Camera::SetViewAngle(float viewAngle)
{
	viewAngle_ = viewAngle;
	XMFLOAT4X4 matProj;
	XMStoreFloat4x4(&matProj, XMMatrixIdentity());
	XMStoreFloat4x4(&matProj, XMMatrixPerspectiveFovLH(viewAngle_, aspect_, nearZ_, (farZ_ > nearZ_) ? farZ_ : nearZ_ + 0.1f));
	proj_ = XMFLOAT4X4ToMatrix4x4(matProj);
}

void Camera::SetPosition(const Vector3D& pos)
{
	eye_ = pos;

	UpdateViewMatrix();
}

void Camera::SetPosition(float posX, float posY, float posZ)
{
	eye_.x = posX;
	eye_.y = posY;
	eye_.z = posZ;

	UpdateViewMatrix();
}

void Camera::MovePosition(const Vector3D& axis)
{
	eye_ += axis;

	UpdateViewMatrix();
}

void Camera::MovePosition(float axisX, float axisY, float axisZ)
{
	eye_.x += axisX;
	eye_.y += axisY;
	eye_.z += axisZ;

	UpdateViewMatrix();
}

void Camera::SetFocus(const Vector3D& pos)
{
	focus_ = pos;

	UpdateViewMatrix();
}

void Camera::SetFocus(float posX, float posY, float posZ)
{
	focus_.x = posX;
	focus_.y = posY;
	focus_.z = posZ;

	UpdateViewMatrix();
}

void Camera::MoveFocus(const Vector3D& axis)
{
	focus_ += axis;

	UpdateViewMatrix();
}

void Camera::MoveFocus(float axisX, float axisY, float axisZ)
{
	focus_.x += axisX;
	focus_.y += axisY;
	focus_.z += axisZ;

	UpdateViewMatrix();
}

void Camera::SetViewPort(float topLeftX, float topLeftY, float width, float height)
{
	viewPort_.topLeftX = topLeftX;
	viewPort_.topLeftY = topLeftY;
	viewPort_.width = width;
	viewPort_.height = height;
	viewPort_.minDepth = 0.0f;
	viewPort_.maxDepth = 1.0f;
}

void Camera::AddObject(Object& object)
{
	Object* pObj = &object;
	auto it = find(vecObjPtr_.begin(), vecObjPtr_.end(), pObj);

	if(it != vecObjPtr_.end()){ return; }

	vecObjPtr_.push_back(pObj);

	auto itc = find(pObj->vecCameraPtr_.begin(), pObj->vecCameraPtr_.end(), this);

	if(itc == pObj->vecCameraPtr_.end()){
		pObj->vecCameraPtr_.push_back(this);
	}
}

void Camera::RemoveObject(Object& object)
{
	Object* pObj = &object;
	auto it = find(vecObjPtr_.begin(), vecObjPtr_.end(), pObj);

	if(it == vecObjPtr_.end()){ return; }

	auto itc = find((*it)->vecCameraPtr_.begin(), (*it)->vecCameraPtr_.end(), this);
	if(itc == (*it)->vecCameraPtr_.end()){ return; }
	(*itc) = nullptr;

	vecObjPtr_.erase(it);
}

void Camera::ResetObject()
{
	if(vecObjPtr_.size() > 0){
		for(auto& obj : vecObjPtr_){
			auto itc = find(obj->vecCameraPtr_.begin(), obj->vecCameraPtr_.end(), this);
			if(itc == obj->vecCameraPtr_.end()){ continue; }
			(*itc) = nullptr;
		}
		vector<Object*>().swap(vecObjPtr_);
	}
}

void Camera::DrawObject()
{
	GraphicManager& gm = GraphicManager::Instance();

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

	for(auto& obj : vecObjPtr_){
		//if(IsVisible(*obj)){
			if(obj->GetColor().a == 1.0f){
				obj->Draw(this);
			}
		//}
	}

	for(auto& obj : vecObjPtr_){
		//if(IsVisible(*obj)){
			if(obj->GetColor().a < 1.0f && obj->GetColor().a != 0.0f){
				obj->Draw(this);
			}
		//}
	}

	gm.EndMask();
}

bool Camera::IsVisible(const Object& object)
{
	UpdateViewMatrix();
	Matrix4x4 vp, invvp;

	vp = view_ * proj_;
	invvp = vp.Inverse();

	Vector3D vpos[4];
	Vector3D wpos[4];

	vpos[0] = Vector3D(-1.0f, 1.0f, 1.0f);
	vpos[1] = Vector3D(1.0f, 1.0f, 1.0f);
	vpos[2] = Vector3D(-1.0f, -1.0f, 1.0f);
	vpos[3] = Vector3D(1.0f, -1.0f, 1.0f);

	wpos[0] = invvp.Vector3DTransform(vpos[0]);
	wpos[1] = invvp.Vector3DTransform(vpos[1]);
	wpos[2] = invvp.Vector3DTransform(vpos[2]);
	wpos[3] = invvp.Vector3DTransform(vpos[3]);

	Vector3D v, v1, v2, n;
	v = object.GetPosition() - eye_;

	// 左
	v1 = wpos[0] - eye_;
	v2 = wpos[2] - eye_;
	n = v1.Cross(v2).Normalized();

	if(n.Dot(v) < -(object.GetScale().Length() * object.GetLengthRate())){
		return false;
	}

	// 右
	v1 = wpos[3] - eye_;
	v2 = wpos[1] - eye_;
	n = v1.Cross(v2).Normalized();

	if(n.Dot(v) < -(object.GetScale().Length() * object.GetLengthRate())){
		return false;
	}

	// 下
	v1 = wpos[2] - eye_;
	v2 = wpos[3] - eye_;
	n = v1.Cross(v2).Normalized();

	if(n.Dot(v) < -(object.GetScale().Length() * object.GetLengthRate())){
		return false;
	}

	// 上
	v1 = wpos[1] - eye_;
	v2 = wpos[0] - eye_;
	n = v1.Cross(v2).Normalized();

	if(n.Dot(v) < -(object.GetScale().Length() * object.GetLengthRate())){
		return false;
	}

	return true;
}

bool Camera::CheckHitRayToSphere(const Object & object)
{
	Vector3D u = eye_ - object.GetPosition();
	Vector3D v = focus_ - eye_;
	float r = object.GetLengthRate();

	float a = v.Dot(v);
	float b = v.Dot(u);
	float c = u.Dot(u) - r * r;

	if(a - 0.0001f <= 0.0f){
		return false;
	}

	float hit = b * b - a * c;
	if(hit < 0.0f) {
		return false;
	}

	float t = (-b - sqrtf(b * b - a * c)) / a;
	if(t > 1.0f){
		return false;
	}

	u = focus_ - object.GetPosition();
	v = eye_ - focus_;
	r = max(max(object.GetScale().x, object.GetScale().y), object.GetScale().z) * 0.5f;

	a = v.Dot(v);
	b = v.Dot(u);
	c = u.Dot(u) - r * r;

	if(a - 0.0001f <= 0.0f){
		return false;
	}

	hit = b * b - a * c;
	if(hit < 0.0f) {
		return false;
	}

	t = (-b - sqrtf(b * b - a * c)) / a;
	if(t > 1.0f){
		return false;
	}

	return true;
}

void Camera::UpdateViewMatrix()
{
	sc_.SetSphere(eye_, radius_, Vector3D());

	XMVECTOR eye = XMVectorSet(eye_.x, eye_.y, eye_.z, 0.0f);
	XMVECTOR focus = XMVectorSet(focus_.x, focus_.y, focus_.z, 0.0f);
	XMVECTOR up = XMVectorSet(up_.x, up_.y, up_.z, 1.0f);

	XMFLOAT4X4 matView;
	XMStoreFloat4x4(&matView, XMMatrixLookAtLH(eye, focus, up));
	view_ = XMFLOAT4X4ToMatrix4x4(matView);
}
