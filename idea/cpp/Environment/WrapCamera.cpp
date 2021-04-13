#include "../../h/Environment/WrapCamera.h"
#include "../../../idea//h/Utility/ideaUtility.h"
#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <directxmath.h>

using namespace DirectX;
using namespace idea;

WrapCamera::WrapCamera() :
	distance_(1.0f),
	pitchThreshold_(-ideaMath::PI * 0.4925f, ideaMath::PI * 0.4925f),
	isWrap_(false){}

void WrapCamera::Init(float viewAngle, float aspect, float nearZ, float farZ, float distance)
{
	nearZ_ = nearZ;
	farZ_ = farZ;

	Camera::Init(viewAngle, aspect, nearZ_, farZ_);

	distance_ = fabsf(max(nearZ, min(farZ, distance)));

	rotation_ = Vector2D(0.0f, 0.0f);

	targetQtn_ = qtn_ = QuaternionFromEuler(0.0f, 0.0f, 0.0f);

	pitchThreshold_ = Vector2D(-ideaMath::PI * 0.4925f, ideaMath::PI * 0.4925f);

	t_ = 0.0f;

	UpdatePosition();
}

void WrapCamera::SetFocus(const Vector3D& pos)
{
	focus_ = pos;

	UpdatePosition();
	UpdateViewMatrix();
}

void WrapCamera::SetFocus(float posX, float posY, float posZ)
{
	SetFocus(Vector3D(posX, posY, posZ));
}

void WrapCamera::MoveFocus(const Vector3D& axis)
{
	focus_ += axis;

	UpdatePosition();
	UpdateViewMatrix();
}

void WrapCamera::MoveFocus(float axisX, float axisY, float axisZ)
{
	MoveFocus(Vector3D(axisX, axisY, axisZ));
}


void WrapCamera::SetDistance(float distance)
{
	distance_ = max(nearZ_, min(farZ_, distance));

	UpdatePosition();
}

void WrapCamera::MoveDistance(float distance)
{
	distance_ += distance;
	distance_ = max(nearZ_, min(farZ_, distance_));

	UpdatePosition();
}

void WrapCamera::SetRotation(Vector2D rot)
{
	rotation_ = rot;
	qtn_ = QuaternionFromEuler(rotation_.y, rotation_.x, 0.0f);

	UpdatePosition();
}

void WrapCamera::SetRotation(float yaw, float pitch)
{
	SetRotation(Vector2D(yaw, pitch));
}

void WrapCamera::MoveRotation(Vector2D rot)
{
	rotation_ += rot;
	qtn_ = QuaternionFromEuler(rotation_.y, rotation_.x, 0.0f);

	UpdatePosition();
}

void WrapCamera::MoveRotation(float yaw, float pitch)
{
	MoveRotation(Vector2D(yaw, pitch));
}

void WrapCamera::MoveRotation(Vector2D rot, float speed)
{
	rot = rot.Normalized() * speed;
	rotation_ += rot;
	qtn_ = QuaternionFromEuler(rotation_.y, rotation_.x, 0.0f);

	UpdatePosition();
}

void WrapCamera::MoveRotation(float yaw, float pitch, float speed)
{
	MoveRotation(Vector2D(yaw, pitch), speed);
}

void WrapCamera::SetPitchThreshold(Vector2D pitch)
{
	if(pitch.x < -ideaMath::PI * 0.4925f){
		pitch.x = -ideaMath::PI * 0.4925f;
	}
	if(pitch.y > ideaMath::PI * 0.4925f){
		pitch.y = ideaMath::PI * 0.4925f;
	}

	if(pitch.x <= pitch.y){
		pitchThreshold_ = pitch;
	}

	UpdatePosition();
}

void WrapCamera::SetPitchThreshold(float pitchMin, float pithMax)
{
	SetPitchThreshold(Vector2D(pitchMin, pithMax));
}

void WrapCamera::SetWrapTarget(Vector2D rot)
{
	if(!isWrap_){
		isWrap_ = true;

		startQtn_ = qtn_;

		// 正規化
		rot.x = fmodf(rot.x, ideaMath::PI * 2.0f);
		if(rot.x < 0.0f){
			rot.x += ideaMath::PI * 2.0f;
		}

		rot.y = fmodf(rot.y, ideaMath::PI * 2.0f);
		if(rot.y < 0.0f){
			rot.y += ideaMath::PI * 2.0f;
		}

		targetQtn_ = QuaternionFromEuler(rot.y, rot.x, 0.0f);
	}
}

void WrapCamera::SetWrapTarget(float yaw, float pitch)
{
	SetWrapTarget(Vector2D(yaw, pitch));
}

void WrapCamera::UpdateWrap(float delta)
{
	if(isWrap_){
		t_ += delta;

		if(t_ < 1.0f){
			qtn_ = QuaternionSLerp(startQtn_, targetQtn_, t_);
		} else {
			isWrap_ = false;
			t_ = 0.0f;
			startQtn_ = qtn_ = targetQtn_;
		}
	} else{
		t_ = 0.0f;
		targetQtn_ = startQtn_ = qtn_;
	}

	UpdatePosition();
}

void WrapCamera::InterruptionWrap()
{
	isWrap_ = false;
	t_ = 0.0f;
	targetQtn_ = startQtn_ = qtn_;
}

Vector2D WrapCamera::FixVector(const Vector2D& vec)
{
	float x = (vec.x * cos(rotation_.x) - vec.y * cos(rotation_.x - ideaMath::PI * 0.5f));
	float y = (vec.y * cos(rotation_.x) + vec.x * cos(rotation_.x - ideaMath::PI * 0.5f));

	return Vector2D(x, y);
}

void WrapCamera::UpdatePosition()
{
	rotation_.x = qtn_.ToEuler().y;
	rotation_.y = qtn_.ToEuler().x;

	// 正規化
	rotation_.x = fmodf(rotation_.x, ideaMath::PI * 2.0f);
	if(rotation_.x < 0.0f){
		rotation_.x += ideaMath::PI * 2.0f;
	}

	rotation_.y = fmodf(rotation_.y, ideaMath::PI * 2.0f);
	if(rotation_.y < 0.0f){
		rotation_.y += ideaMath::PI * 2.0f;
	}

	// 閾値
	Vector2D tmpRot = rotation_;

	if(tmpRot.y > ideaMath::PI){
		tmpRot.y = tmpRot.y - ideaMath::PI * 2.0f;
	}

	if(tmpRot.y < pitchThreshold_.x){
		rotation_.y = pitchThreshold_.x + ideaMath::PI * 2.0f;
		while(rotation_.y > ideaMath::PI * 2.0f)
		{
			rotation_.y -= ideaMath::PI * 2.0f;
		}
	}else if(tmpRot.y > pitchThreshold_.y){
		rotation_.y = pitchThreshold_.y;
	}

	// upの修正
	if(rotation_.y < ideaMath::PI * 0.5f){
		up_.y = 1.0f;
	} else if(rotation_.y < ideaMath::PI * 1.5f){
		up_.y = -1.0f;
	} else{
		up_.y = 1.0f;
	}

	// カメラポジションの更新
	eye_ = focus_;
	eye_.x += distance_ * sinf(rotation_.x) * cosf(rotation_.y);
	eye_.y += distance_ * sinf(rotation_.y);
	eye_.z += distance_ * -cosf(rotation_.x) * cosf(rotation_.y);

	UpdateViewMatrix();
}