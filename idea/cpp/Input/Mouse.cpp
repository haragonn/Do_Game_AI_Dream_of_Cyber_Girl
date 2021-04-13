#include "../../h/Input/Mouse.h"
#include "../../h/Input/InputManager.h"
#include "../../h/Framework/GraphicManager.h"
#include "../../h/3D/Object.h"
#include "../../h/Utility/ideaUtility.h"

using namespace idea;

Vector2D Mouse::GetPosition()
{
	auto& mouse = InputManager::Instance().GetMouse();

	return Vector2D(static_cast<float>(mouse.posX), static_cast<float>(mouse.posY));
}

Vector2D idea::Mouse::GetDifference()
{
	auto& mouse = InputManager::Instance().GetMouse();

	return Vector2D(static_cast<float>(mouse.differenceX), static_cast<float>(mouse.differenceY));
}

int Mouse::GetButton(unsigned int buttonID)
{
	Assert(buttonID < BUTTON_MAX);

	return InputManager::Instance().GetMouse().button[buttonID];
}

float Mouse::GetWheel()
{
	return static_cast<float>(InputManager::Instance().GetMouse().wheel);
}

Segment3D Mouse::SegmentToWorld(const Camera& camera)
{
	Matrix4x4 vpMat;

	GraphicManager& gm = GraphicManager::Instance();

	HWND hWnd = gm.GetHWND();

	RECT rect;

	GetClientRect(hWnd, &rect);

	Camera::ViewPort vp = camera.GetViewPort();

	float width = vp.width * (rect.right - rect.left) / gm.GetWidth();
	float height = vp.height * (rect.bottom - rect.top) / gm.GetHeight();

	vpMat._11 = (float)width / 2.0f;
	vpMat._22 = -1.0f * (float)(height / 2.0f);
	vpMat._33 = (float)vp.maxDepth - vp.minDepth;
	vpMat._41 = (float)(vp.topLeftX + width / 2.0f);
	vpMat._42 = (float)(vp.topLeftY + height / 2.0f);
	vpMat._43 = vp.minDepth;

	Matrix4x4 invMat, inv_proj, inv_view;

	invMat = vpMat.Inverse();
	inv_proj = camera.GetProjectionMatrix().Inverse();
	inv_view = camera.GetViewMatrix().Inverse();

	invMat *= inv_proj * inv_view;

	Vector3D sp = { GetPosition() , 0.0f };

	sp = invMat.Vector3DTransform(sp);

	Vector3D ep = { GetPosition() , 1.0f };

	ep = invMat.Vector3DTransform(ep);

	return Segment3D(sp, ep);
}

Vector3D idea::Mouse::PositionOnSegmentFromOBB(const Camera& camera, BoxCollider bc)
{
	Matrix4x4 vpMat;

	Camera::ViewPort vp = camera.GetViewPort();

	vpMat._11 = (float)vp.width / 2.0f;
	vpMat._22 = -1.0f * (float)(vp.height / 2.0f);
	vpMat._33 = (float)vp.maxDepth - vp.minDepth;
	vpMat._41 = (float)(vp.topLeftX + vp.width / 2.0f);
	vpMat._42 = (float)(vp.topLeftY + vp.height / 2.0f);
	vpMat._43 = vp.minDepth;

	Matrix4x4 invMat, inv_proj, inv_view;

	invMat = vpMat.Inverse();
	inv_proj = camera.GetProjectionMatrix().Inverse();
	inv_view = camera.GetViewMatrix().Inverse();

	invMat *= inv_proj * inv_view;

	Vector3D sp = { GetPosition() , 0.0f };

	sp = invMat.Vector3DTransform(sp);

	return bc.GetClosetPoint(sp);
}

bool idea::Mouse::CheckHitRayToSphere(const Camera& camera, const Object& object)
{
	Segment3D segment = SegmentToWorld(camera);

	Vector3D u = segment.startPoint - object.GetPosition();
	Vector3D v = segment.endPoint - segment.startPoint;

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

	u = segment.endPoint - object.GetPosition();
	v = segment.startPoint - segment.endPoint;
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
