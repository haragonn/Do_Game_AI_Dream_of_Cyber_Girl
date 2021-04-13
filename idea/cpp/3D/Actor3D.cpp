/*==============================================================================
	[Actor3D.cpp]
														Author	:	Keigo Hara
==============================================================================*/
#include "../../h/3D/Actor3D.h"
#include "../../h/3D/Actor3DManager.h"
#include "../../h/Utility/ideaUtility.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <DirectXMath.h>

//------------------------------------------------------------------------------
// using namespace
//------------------------------------------------------------------------------
using namespace std;
using namespace DirectX;
using namespace idea;

//------------------------------------------------------------------------------
// コンストラクタ
//------------------------------------------------------------------------------
Actor3D::Actor3D() :
	lengthRate_(1.0f),
	bRegistered_(false)
{
	material_.ambient = ideaColor::WHITE;
	material_.diffuse = ideaColor::WHITE;
	material_.specular = ideaColor::WHITE;
	material_.shininess = 1.0f;

	UpdateWorldMatrix();

	Actor3DManager::Instance().Register(this);
}

//------------------------------------------------------------------------------
// デストラクタ
//------------------------------------------------------------------------------
Actor3D::~Actor3D()
{
	Actor3DManager::Instance().UnRegister(this);
}

//------------------------------------------------------------------------------
// 初期化
//------------------------------------------------------------------------------
void Actor3D::Init(const idea::Vector3D& position)
{
	position_ = position;

	prePosition_ = position;

	scale_ = Vector3D(1.0f, 1.0f, 1.0f);

	rotation_ = Vector3DZero();

	lengthRate_ = 1.0f;

	material_.diffuse = ideaColor::WHITE;
	material_.ambient = ideaColor::WHITE;
	material_.specular = ideaColor::WHITE;
	material_.shininess = 1.0f;

	UpdateWorldMatrix();
}

void Actor3D::Init(float positionX, float positionY, float positionZ)
{
	position_ = Vector3D(positionX, positionY, positionZ);

	prePosition_ = Vector3D(positionX, positionY, positionZ);

	scale_ = Vector3D(1.0f, 1.0f, 1.0f);

	rotation_ = Vector3DZero();

	lengthRate_ = 1.0f;

	material_.ambient = ideaColor::WHITE;
	material_.diffuse = ideaColor::WHITE;
	material_.specular = ideaColor::WHITE;
	material_.shininess = 1.0f;

	UpdateWorldMatrix();
}

//------------------------------------------------------------------------------
// 終了
//------------------------------------------------------------------------------
void Actor3D::UnInit()
{
	position_ = Vector3DZero();

	scale_ = Vector3DZero();

	rotation_ = Vector3DZero();

	lengthRate_ = 0.0f;

	UpdateWorldMatrix();

	vector<Actor3D*>().swap(children_);
}

//------------------------------------------------------------------------------
// 座標の設定
//------------------------------------------------------------------------------
void Actor3D::SetPosition(const idea::Vector3D& position)
{
	position_ = position;

	UpdateWorldMatrix();
}

void Actor3D::SetPosition(float positionX, float positionY, float positionZ)
{
	SetPosition(Vector3D(positionX, positionY, positionZ));
}

void idea::Actor3D::SetBasePosition(const Vector3D& position)
{
	prePosition_ = position_ = position;

	UpdateWorldMatrix();
}

void idea::Actor3D::SetBasePosition(float positionX, float positionY, float positionZ)
{
	SetBasePosition(Vector3D(positionX, positionY, positionZ));
}

//------------------------------------------------------------------------------
// 座標の移動
//------------------------------------------------------------------------------
void Actor3D::MovePosition(const idea::Vector3D& axis)
{
	position_ += axis;

	UpdateWorldMatrix();
}

void Actor3D::MovePosition(float axisX, float axisY, float axisZ)
{
	MovePosition(Vector3D(axisX, axisY, axisX));
}

//------------------------------------------------------------------------------
// 回転角の設定
//------------------------------------------------------------------------------
void Actor3D::SetRotation(const idea::Vector3D& rot)
{
	rotation_ = rot;

	UpdateWorldMatrix();
}

void Actor3D::SetRotation(float pitch, float yaw, float roll)
{
	SetRotation(Vector3D(pitch, yaw, roll));
}

//------------------------------------------------------------------------------
// 回転
//------------------------------------------------------------------------------
void Actor3D::MoveRotation(const idea::Vector3D& rot)
{
	rotation_ += rot;

	UpdateWorldMatrix();
}

void Actor3D::MoveRotation(float pitch, float yaw, float roll)
{
	MoveRotation(Vector3D(pitch, yaw, roll));
}

//------------------------------------------------------------------------------
// 拡縮倍率の設定
//------------------------------------------------------------------------------
void Actor3D::SetScale(const idea::Vector3D& scale)
{
	scale_ = scale;

	lengthRate_ = scale_.Length();

	UpdateWorldMatrix();
}

void Actor3D::SetScale(float scaleX, float scaleY, float scaleZ)
{
	SetScale(Vector3D(scaleX, scaleY, scaleZ));
}

//------------------------------------------------------------------------------
// 色の設定
//------------------------------------------------------------------------------
void Actor3D::SetColor(const idea::Color& color)
{
	color_.r = Clamp(color.r, 0.0f, 1.0f);
	color_.g = Clamp(color.g, 0.0f, 1.0f);
	color_.b = Clamp(color.b, 0.0f, 1.0f);
	color_.a = Clamp(color.a, 0.0f, 1.0f);
}

void Actor3D::SetColor(float r, float g, float b, float a)
{
	color_.r = Clamp(r, 0.0f, 1.0f);
	color_.g = Clamp(g, 0.0f, 1.0f);
	color_.b = Clamp(b, 0.0f, 1.0f);
	color_.a = Clamp(a, 0.0f, 1.0f);
}

//------------------------------------------------------------------------------
// 色の設定
//------------------------------------------------------------------------------
void Actor3D::SetDiffuse(const idea::Color& color)
{
	material_.diffuse.r = Clamp(color.r, 0.0f, 1.0f);
	material_.diffuse.g = Clamp(color.g, 0.0f, 1.0f);
	material_.diffuse.b = Clamp(color.b, 0.0f, 1.0f);
	material_.diffuse.a = Clamp(color.a, 0.0f, 1.0f);
}

void Actor3D::SetDiffuse(float r, float g, float b, float a)
{
	material_.diffuse.r = Clamp(r, 0.0f, 1.0f);
	material_.diffuse.g = Clamp(g, 0.0f, 1.0f);
	material_.diffuse.b = Clamp(b, 0.0f, 1.0f);
	material_.diffuse.a = Clamp(a, 0.0f, 1.0f);
}

//------------------------------------------------------------------------------
// 色の設定
//------------------------------------------------------------------------------
void Actor3D::SetAmbient(const idea::Color& color)
{
	material_.ambient.r = Clamp(color.r, 0.0f, 1.0f);
	material_.ambient.g = Clamp(color.g, 0.0f, 1.0f);
	material_.ambient.b = Clamp(color.b, 0.0f, 1.0f);
	material_.ambient.a = Clamp(color.a, 0.0f, 1.0f);
}

void Actor3D::SetAmbient(float r, float g, float b, float a)
{
	material_.ambient.r = Clamp(r, 0.0f, 1.0f);
	material_.ambient.g = Clamp(g, 0.0f, 1.0f);
	material_.ambient.b = Clamp(b, 0.0f, 1.0f);
	material_.ambient.a = Clamp(a, 0.0f, 1.0f);
}

//------------------------------------------------------------------------------
// 色の設定
//------------------------------------------------------------------------------
void Actor3D::SetSpecular(const idea::Color& color)
{
	material_.specular.r = Clamp(color.r, 0.0f, 1.0f);
	material_.specular.g = Clamp(color.g, 0.0f, 1.0f);
	material_.specular.b = Clamp(color.b, 0.0f, 1.0f);
	material_.specular.a = Clamp(color.a, 0.0f, 1.0f);
}

void Actor3D::SetSpecular(float r, float g, float b, float a)
{
	material_.specular.r = Clamp(r, 0.0f, 1.0f);
	material_.specular.g = Clamp(g, 0.0f, 1.0f);
	material_.specular.b = Clamp(b, 0.0f, 1.0f);
	material_.specular.a = Clamp(a, 0.0f, 1.0f);
}

void Actor3D::SetShininess(float shininess)
{
	material_.shininess = shininess;
}

void idea::Actor3D::SetChild(Actor3D& child)
{
	if(find(children_.begin(), children_.end(), (&child)) != children_.end()){
		return;
	}

	children_.push_back((&child));
}

//------------------------------------------------------------------------------
// ワールドマトリクスの更新
//------------------------------------------------------------------------------
inline void Actor3D::UpdateWorldMatrix()
{
	XMMATRIX matWorld, matScale, matRot;

	// ワールド行列の初期化
	matWorld = XMMatrixIdentity();

	// 拡大縮小処理
	matScale = XMMatrixScaling(scale_.x, scale_.y, scale_.z);

	// 回転の正規化
	rotation_.x = WrapRadian(rotation_.x);
	rotation_.y = WrapRadian(rotation_.y);
	rotation_.z = WrapRadian(rotation_.z);

	// 回転処理
	matRot = XMMatrixRotationRollPitchYaw(rotation_.x, rotation_.y, rotation_.z);

	// 行列の合成
	matWorld = XMMatrixMultiply(matWorld, matScale);
	matWorld = XMMatrixMultiply(matWorld, matRot);

	// 平行移動
	matWorld.r[3].m128_f32[0] = position_.x;
	matWorld.r[3].m128_f32[1] = position_.y;
	matWorld.r[3].m128_f32[2] = position_.z;

	// 格納
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			rotationMatrix_.r[i][j] = matRot.r[i].m128_f32[j];
			world_.r[i][j] = matWorld.r[i].m128_f32[j];
		}
	}

	if(children_.size()){
		for(auto& child : children_){
			child->UpdateWorldMatrix(world_);
		}
	}
}

inline void idea::Actor3D::UpdateWorldMatrix(Matrix4x4& parentMatrix)
{
	XMMATRIX matWorld, matScale, matRot;

	// ワールド行列の初期化
	matWorld = XMMatrixIdentity();

	// 拡大縮小処理
	matScale = XMMatrixScaling(scale_.x, scale_.y, scale_.z);

	// 回転の正規化
	rotation_.x = WrapRadian(rotation_.x);
	rotation_.y = WrapRadian(rotation_.y);
	rotation_.z = WrapRadian(rotation_.z);

	// 回転処理
	matRot = XMMatrixRotationRollPitchYaw(rotation_.x, rotation_.y, rotation_.z);

	// 行列の合成
	matWorld = XMMatrixMultiply(matWorld, matScale);
	matWorld = XMMatrixMultiply(matWorld, matRot);

	// 平行移動
	matWorld.r[3].m128_f32[0] = position_.x;
	matWorld.r[3].m128_f32[1] = position_.y;
	matWorld.r[3].m128_f32[2] = position_.z;

	// 格納
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			world_.r[i][j] = matWorld.r[i].m128_f32[j];
		}
	}

	world_ *= parentMatrix;

	if(children_.size()){
		for(auto& child : children_){
			child->UpdateWorldMatrix(world_);
		}
	}
}

void idea::Actor3D::UpdatePrePosition()
{
	// 前フレームの位置を保存
	prePosition_ = position_;
}
