/*==============================================================================
	[Actor2D.cpp]
														Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "../../h/2D/Actor2D.h"
#include "../../h/2D/Actor2DManager.h"
#include "../../h/Utility/ideaMath.h"

#include <algorithm>

//------------------------------------------------------------------------------
// using namespace
//------------------------------------------------------------------------------
using namespace std;
using namespace idea;

//------------------------------------------------------------------------------
// �R���X�g���N�^
//------------------------------------------------------------------------------
Actor2D::Actor2D() :
	radian_(0.0f),
	bReversedU_(false),
	bReversedV_(false),
	bRegistered_(false),
	pLeader_(nullptr),
	pFollower_(nullptr)
{
	// Actor2DManager�̓o�^
	Actor2DManager::Instance().Register(this);
}

//------------------------------------------------------------------------------
// �f�X�g���N�^
//------------------------------------------------------------------------------
Actor2D::~Actor2D()
{
	UnInit();

	// Actor2DManager�̓o�^����
	Actor2DManager::Instance().UnRegister(this);
}

//------------------------------------------------------------------------------
// ������
//------------------------------------------------------------------------------
void Actor2D::Init(const Vector2D& position, const Vector2D& size)
{
	Init(position.x, position.y, size.x, size.y);
}

void Actor2D::Init(const Vector2D& position, float width, float height)
{
	Init(position.x, position.y, width, height);
}

void Actor2D::Init(float positionX, float positionY, const Vector2D& size)
{
	Init(positionX, positionY, size.x, size.y);
}

void Actor2D::Init(float posX, float posY, float width, float height)
{
	position_ = Vector2D(posX, posY);

	prePosition_ = Vector2D(posX, posY);

	size_ = Vector2D(width, height);

	radian_ = 0.0f;

	// �l���}�C�i�X�̎��͔��]�t���O���I���ɂ��Đ����ɂ���
	bReversedU_ = false;

	if(size_.x < 0.0f){
		size_.x *= -1.0f;
		bReversedU_ = true;
	}

	bReversedV_ = false;

	if(size_.y < 0.0f){
		size_.y *= -1.0f;
		bReversedV_ = true;
	}

	color_ = ideaColor::WHITE;
}

//------------------------------------------------------------------------------
// �I��
//------------------------------------------------------------------------------
void Actor2D::UnInit()
{
	// �[���N���A
	position_ = Vector2D();

	prePosition_ = Vector2D();

	size_ = Vector2D();

	radian_ = 0.0f;

	bReversedU_ = false;

	bReversedV_ = false;

	color_ = ideaColor::CLEAR;

	// �t�H���[�֌W�̉��U
	if(pLeader_){
		QuitFollower();
	} else{
		ResetFollower(pFollower_);
	}
}
//------------------------------------------------------------------------------
// �ʒu�̐ݒ�
//------------------------------------------------------------------------------
void Actor2D::SetPosition(const Vector2D& pos)
{
	// �ʒu�̕ێ�
	Vector2D tmpPosition = position_;

	position_ = pos;

	// ���[�_�[�Ȃ�΃t�H�����[���ړ�������
	if(pFollower_ && !pLeader_){
		pFollower_->MoveFollower(position_ - tmpPosition);
	}
}

void Actor2D::SetPosition(float posX, float posY)
{
	// �ʒu�̕ێ�
	Vector2D tmpPosition = position_;

	position_ = Vector2D(posX, posY);

	// ���[�_�[�Ȃ�΃t�H�����[���ړ�������
	if(pFollower_ && !pLeader_){
		pFollower_->MoveFollower(position_ - tmpPosition);
	}
}

//------------------------------------------------------------------------------
// �O�t���[���̍��W���ƍ��W��ݒ�
//------------------------------------------------------------------------------
void Actor2D::SetBasePosition(const Vector2D& position)
{
	// �ʒu�̕ێ�
	Vector2D tmpPosition = position_;

	position_ = position;

	prePosition_ = position;

	// ���[�_�[�Ȃ�΃t�H�����[���ړ�������
	if(pFollower_ && !pLeader_){
		pFollower_->MoveFollower(position_ - tmpPosition);
	}
}

void Actor2D::SetBasePosition(float posX, float posY)
{
	// �ʒu�̕ێ�
	Vector2D tmpPosition = position_;

	position_ = Vector2D(posX, posY);

	prePosition_ = Vector2D(posX, posY);

	// ���[�_�[�Ȃ�΃t�H�����[���ړ�������
	if(pFollower_ && !pLeader_){
		pFollower_->MoveFollower(position_ - tmpPosition);
	}
}

//------------------------------------------------------------------------------
// �ʒu�̈ړ�
//------------------------------------------------------------------------------
void Actor2D::MovePosition(const Vector2D& axis)
{
	position_ += axis;

	// ���[�_�[�Ȃ�΃t�H�����[���ړ�������
	if(pFollower_ && !pLeader_){
		pFollower_->MoveFollower(axis);
	}
}

void Actor2D::MovePosition(float axisX, float axisY)
{
	Vector2D axis = Vector2D(axisX, axisY);

	position_ += Vector2D(axisX, axisY);

	// ���[�_�[�Ȃ�΃t�H�����[���ړ�������
	if(pFollower_ && !pLeader_){
		pFollower_->MoveFollower(axis);
	}
}

//------------------------------------------------------------------------------
// �傫���̐ݒ�
//------------------------------------------------------------------------------
void Actor2D::SetSize(const Vector2D& size)
{
	size_ = size;

	// �l���}�C�i�X�̎��͔��]�t���O���I���ɂ��Đ����ɂ���
	bReversedU_ = false;

	if(size_.x < 0.0f){
		size_.x *= -1.0f;
		bReversedU_ = true;
	}

	bReversedV_ = false;

	if(size_.y < 0.0f){
		size_.y *= -1.0f;
		bReversedV_ = true;
	}
}

void Actor2D::SetSize(float width, float height)
{
	size_ = Vector2D(width, height);

	// �l���}�C�i�X�̎��͔��]�t���O���I���ɂ��Đ����ɂ���
	bReversedU_ = false;

	if(size_.x < 0.0f){
		size_.x *= -1.0f;
		bReversedU_ = true;
	}

	bReversedV_ = false;

	if(size_.y < 0.0f){
		size_.y *= -1.0f;
		bReversedV_ = true;
	}
}

//------------------------------------------------------------------------------
// ��]�p�̐ݒ�
//------------------------------------------------------------------------------
void Actor2D::SetRotation(float radian)
{
	radian_ = WrapRadian(radian);
}

//------------------------------------------------------------------------------
// ��]
//------------------------------------------------------------------------------
void Actor2D::MoveRotation(float radian)
{
	radian_ += radian;

	radian_ = WrapRadian(radian_);
}

//------------------------------------------------------------------------------
// �F�̐ݒ�
//------------------------------------------------------------------------------
void Actor2D::SetColor(const Color& color)
{
	color_.r = Clamp(color.r, 0.0f, 1.0f);
	color_.g = Clamp(color.g, 0.0f, 1.0f);
	color_.b = Clamp(color.b, 0.0f, 1.0f);
	color_.a = Clamp(color.a, 0.0f, 1.0f);
}

void Actor2D::SetColor(float r, float g, float b, float a)
{
	color_.r = Clamp(r, 0.0f, 1.0f);
	color_.g = Clamp(g, 0.0f, 1.0f);
	color_.b = Clamp(b, 0.0f, 1.0f);
	color_.a = Clamp(a, 0.0f, 1.0f);
}

//------------------------------------------------------------------------------
// �~�̐ڐG����
//------------------------------------------------------------------------------
bool Actor2D::CheckHitCircle(const Actor2D& target)const
{
	// ���_���
	float x = position_.x - target.position_.x;
	float y = position_.y - target.position_.y;
	float r = max(size_.x * 0.5f, size_.y * 0.5f) + max(target.size_.x * 0.5f, target.size_.y * 0.5f);	// ���ƍ����̑傫�������w�W�Ƃ���

	// �~�̔���
	if(r == 0.0f){
		return false;
	}

	if((x * x) + (y * y) < r * r){
		return true;
	}

	// �ړ��ʂ̌v�Z
	float movementX = (position_.x - prePosition_.x) + (target.position_.x - target.prePosition_.x);
	float movementY = (position_.y - prePosition_.y) + (target.position_.y - target.prePosition_.y);
	movementX = abs(movementX);
	movementY = abs(movementY);

	// �����蔻����щz���Ă��Ȃ���
	if(movementX < r && movementY < r){
		return false;
	}

	// �����蔻��̑傫���ɍ��킹�ړ��ʂ𕪉�����
	int division = 1;

	if(movementX > movementY){
		division = (int)(movementX / r) + 1;
	} else{
		division = (int)(movementY / r) + 1;
	}

	if(division == 0){
		return false;
	}

	float axisX1 = (position_.x - prePosition_.x) / division;
	float axisY1 = (position_.y - prePosition_.y) / division;
	float axisX2 = (target.position_.x - target.prePosition_.x) / division;
	float axisY2 = (target.position_.y - target.prePosition_.y) / division;

	// �O�t���[���̒��_���
	float posX1 = prePosition_.x;
	float posY1 = prePosition_.y;
	float posX2 = target.prePosition_.x;
	float posY2 = target.prePosition_.y;

	// �������������Ȃ��画��
	for(int i = 0; i < division; ++i){
		posX1 += axisX1;
		posY1 += axisY1;
		posX2 += axisX2;
		posY2 += axisY2;
		x = posX1 - posX2;
		y = posY1 - posY2;

		// �~�̔���
		if((x * x) + (y * y) < r * r){
			return true;
		}
	}

	return false;
}

//------------------------------------------------------------------------------
// ��`�̐ڐG����
//------------------------------------------------------------------------------
bool Actor2D::CheckHitRect(const Actor2D& target)const
{
	if(abs(radian_) < ideaMath::EPSILON && abs(target.radian_) < ideaMath::EPSILON){	// ��]���Ă��Ȃ���`���m���ǂ���
		// ���_���
		float x1 = position_.x - size_.x * 0.5f;
		float x2 = position_.x + size_.x * 0.5f;
		float y1 = position_.y - size_.y * 0.5f;
		float y2 = position_.y + size_.y * 0.5f;
		float x3 = target.position_.x - target.size_.x * 0.5f;
		float x4 = target.position_.x + target.size_.x * 0.5f;
		float y3 = target.position_.y - target.size_.y * 0.5f;
		float y4 = target.position_.y + target.size_.y * 0.5f;

		// ��`����
		if(x1 < x4 && x2 > x3 && y1 < y4 && y2 > y3){
			return true;
		}

		// �ړ��ʂ̌v�Z
		float movementX = fabsf((position_.x - prePosition_.x) + (target.position_.x - target.prePosition_.x));
		float movementY = fabsf((position_.y - prePosition_.y) + (target.position_.y - target.prePosition_.y));

		// �����蔻����щz���Ă��Ȃ���
		float width = size_.x + target.size_.x;
		float height = size_.y + target.size_.y;

		if(movementX < width && movementY < height){
			return false;
		}

		// �����蔻��̑傫���ɍ��킹�ړ��ʂ𕪉�����
		int division = 1;

		if(movementX - size_.x > movementY - size_.y){
			division = (int)(movementX / size_.x) + 1;
		} else{
			division = (int)(movementY / size_.y) + 1;
		}

		float axisX1 = (position_.x - prePosition_.x) / division;
		float axisY1 = (position_.y - prePosition_.y) / division;
		float axisX2 = (target.position_.x - target.prePosition_.x) / division;
		float axisY2 = (target.position_.y - target.prePosition_.y) / division;

		// �O�t���[���̒��_���
		x1 = prePosition_.x - size_.x * 0.5f;
		x2 = prePosition_.x + size_.x * 0.5f;
		y1 = prePosition_.y - size_.y * 0.5f;
		y2 = prePosition_.y + size_.y * 0.5f;
		x3 = target.prePosition_.x - target.size_.x * 0.5f;
		x4 = target.prePosition_.x + target.size_.x * 0.5f;
		y3 = target.prePosition_.y - target.size_.y * 0.5f;
		y4 = target.prePosition_.y + target.size_.y * 0.5f;

		// �������������Ȃ��画��
		for(int i = 0; i < division; ++i){
			x1 += axisX1;
			x2 += axisX1;
			y1 += axisY1;
			y2 += axisY1;
			x3 += axisX2;
			x4 += axisX2;
			y3 += axisY2;
			y4 += axisY2;

			// ��`����
			if(x1 < x4 && x2 > x3 && y1 < y4 && y2 > y3){
				return true;
			}
		}
	} else{
		// ���_���̌v�Z(����{�Ƃ���)
		float vx[4] = {};
		float vy[4] = {};
		float x, y, axisX, axisY;
		float tSin = sinf(radian_);
		float tCos = cosf(radian_);

		for(int i = 4 - 1; i >= 0; --i){
			x = (i % 2) ? size_.x * 0.5f : -(size_.x * 0.5f);
			y = (i < 2) ? size_.y * 0.5f : -(size_.y * 0.5f);
			axisX = x * tCos - y * tSin;
			axisY = x * tSin + y * tCos;
			vx[i] = axisX + position_.x;
			vy[i] = axisY + position_.y;
		}

		float tvx[4] = {};
		float tvy[4] = {};
		tSin = sinf(target.radian_);
		tCos = cosf(target.radian_);

		for(int i = 4 - 1; i >= 0; --i){
			x = (i % 2) ? target.size_.x * 0.5f : -(target.size_.x * 0.5f);
			y = (i < 2) ? target.size_.y * 0.5f : -(target.size_.y * 0.5f);
			axisX = x * tCos - y * tSin;
			axisY = x * tSin + y * tCos;
			tvx[i] = axisX + target.position_.x;
			tvy[i] = axisY + target.position_.y;
		}

		// �O�ς��炨�݂��̒��_�������ɂ��邩���肷��
		// �E���x�N�g��
		Vector2D vec1 = Vector2D(vx[1], vy[1]) - Vector2D(vx[0], vy[0]);
		Vector2D vec2 = Vector2D(vx[3], vy[3]) - Vector2D(vx[1], vy[1]);
		Vector2D vec3 = Vector2D(vx[2], vy[2]) - Vector2D(vx[3], vy[3]);
		Vector2D vec4 = Vector2D(vx[0], vy[0]) - Vector2D(vx[2], vy[2]);

		// ���_���璸�_�ւ̃x�N�g��
		Vector2D vec11 = Vector2D(tvx[0], tvy[0]) - Vector2D(vx[0], vy[0]);
		Vector2D vec21 = Vector2D(tvx[0], tvy[0]) - Vector2D(vx[1], vy[1]);
		Vector2D vec31 = Vector2D(tvx[0], tvy[0]) - Vector2D(vx[3], vy[3]);
		Vector2D vec41 = Vector2D(tvx[0], tvy[0]) - Vector2D(vx[2], vy[2]);
		Vector2D vec12 = Vector2D(tvx[1], tvy[1]) - Vector2D(vx[0], vy[0]);
		Vector2D vec22 = Vector2D(tvx[1], tvy[1]) - Vector2D(vx[1], vy[1]);
		Vector2D vec32 = Vector2D(tvx[1], tvy[1]) - Vector2D(vx[3], vy[3]);
		Vector2D vec42 = Vector2D(tvx[1], tvy[1]) - Vector2D(vx[2], vy[2]);
		Vector2D vec13 = Vector2D(tvx[2], tvy[2]) - Vector2D(vx[0], vy[0]);
		Vector2D vec23 = Vector2D(tvx[2], tvy[2]) - Vector2D(vx[1], vy[1]);
		Vector2D vec33 = Vector2D(tvx[2], tvy[2]) - Vector2D(vx[3], vy[3]);
		Vector2D vec43 = Vector2D(tvx[2], tvy[2]) - Vector2D(vx[2], vy[2]);
		Vector2D vec14 = Vector2D(tvx[3], tvy[3]) - Vector2D(vx[0], vy[0]);
		Vector2D vec24 = Vector2D(tvx[3], tvy[3]) - Vector2D(vx[1], vy[1]);
		Vector2D vec34 = Vector2D(tvx[3], tvy[3]) - Vector2D(vx[3], vy[3]);
		Vector2D vec44 = Vector2D(tvx[3], tvy[3]) - Vector2D(vx[2], vy[2]);

		// ���肪���g�̓����ɂ��邩
		if(vec1.Cross(vec11) <= 0.0f &&
			vec2.Cross(vec21) <= 0.0f &&
			vec3.Cross(vec31) <= 0.0f &&
			vec4.Cross(vec41) <= 0.0f ||
			vec1.Cross(vec12) <= 0.0f &&
			vec2.Cross(vec22) <= 0.0f &&
			vec3.Cross(vec32) <= 0.0f &&
			vec4.Cross(vec42) <= 0.0f ||
			vec1.Cross(vec13) <= 0.0f &&
			vec2.Cross(vec23) <= 0.0f &&
			vec3.Cross(vec33) <= 0.0f &&
			vec4.Cross(vec43) <= 0.0f ||
			vec1.Cross(vec14) <= 0.0f &&
			vec2.Cross(vec24) <= 0.0f &&
			vec3.Cross(vec34) <= 0.0f &&
			vec4.Cross(vec44) <= 0.0f){
			return true;
		}

		// �E���x�N�g��
		vec1 = Vector2D(tvx[1], tvy[1]) - Vector2D(tvx[0], tvy[0]);
		vec2 = Vector2D(tvx[3], tvy[3]) - Vector2D(tvx[1], tvy[1]);
		vec3 = Vector2D(tvx[2], tvy[2]) - Vector2D(tvx[3], tvy[3]);
		vec4 = Vector2D(tvx[0], tvy[0]) - Vector2D(tvx[2], tvy[2]);

		// ���_���璸�_�ւ̃x�N�g��
		vec11 = Vector2D(vx[0], vy[0]) - Vector2D(tvx[0], tvy[0]);
		vec21 = Vector2D(vx[0], vy[0]) - Vector2D(tvx[1], tvy[1]);
		vec31 = Vector2D(vx[0], vy[0]) - Vector2D(tvx[3], tvy[3]);
		vec41 = Vector2D(vx[0], vy[0]) - Vector2D(tvx[2], tvy[2]);
		vec12 = Vector2D(vx[1], vy[1]) - Vector2D(tvx[0], tvy[0]);
		vec22 = Vector2D(vx[1], vy[1]) - Vector2D(tvx[1], tvy[1]);
		vec32 = Vector2D(vx[1], vy[1]) - Vector2D(tvx[3], tvy[3]);
		vec42 = Vector2D(vx[1], vy[1]) - Vector2D(tvx[2], tvy[2]);
		vec13 = Vector2D(vx[2], vy[2]) - Vector2D(tvx[0], tvy[0]);
		vec23 = Vector2D(vx[2], vy[2]) - Vector2D(tvx[1], tvy[1]);
		vec33 = Vector2D(vx[2], vy[2]) - Vector2D(tvx[3], tvy[3]);
		vec43 = Vector2D(vx[2], vy[2]) - Vector2D(tvx[2], tvy[2]);
		vec14 = Vector2D(vx[3], vy[3]) - Vector2D(tvx[0], tvy[0]);
		vec24 = Vector2D(vx[3], vy[3]) - Vector2D(tvx[1], tvy[1]);
		vec34 = Vector2D(vx[3], vy[3]) - Vector2D(tvx[3], tvy[3]);
		vec44 = Vector2D(vx[3], vy[3]) - Vector2D(tvx[2], tvy[2]);

		// ���g������̓����ɂ��邩
		if(vec1.Cross(vec11) <= 0.0f &&
			vec2.Cross(vec21) <= 0.0f &&
			vec3.Cross(vec31) <= 0.0f &&
			vec4.Cross(vec41) <= 0.0f ||
			vec1.Cross(vec12) <= 0.0f &&
			vec2.Cross(vec22) <= 0.0f &&
			vec3.Cross(vec32) <= 0.0f &&
			vec4.Cross(vec42) <= 0.0f ||
			vec1.Cross(vec13) <= 0.0f &&
			vec2.Cross(vec23) <= 0.0f &&
			vec3.Cross(vec33) <= 0.0f &&
			vec4.Cross(vec43) <= 0.0f ||
			vec1.Cross(vec14) <= 0.0f &&
			vec2.Cross(vec24) <= 0.0f &&
			vec3.Cross(vec34) <= 0.0f &&
			vec4.Cross(vec44) <= 0.0f){
			return true;
		}
	}

	return false;
}

//------------------------------------------------------------------------------
// �ʒu�֌W�擾
//------------------------------------------------------------------------------
float Actor2D::GetRelation(const Actor2D& target)const
{
	// �d�Ȃ��Ă���ꍇ0.0f��Ԃ�
	if(target.position_.x - position_.x == 0.0f && target.position_.y - position_.y == 0.0f){
		return 0.0f;
	}

	// ���0�Ƃ��Ď��v���Ɉʒu�֌W��Ԃ�
	return ideaMath::PI - atan2f(target.position_.x - position_.x, target.position_.y - position_.y);
}

//------------------------------------------------------------------------------
// �O�t���[���̈ʒu�֌W�擾
//------------------------------------------------------------------------------
float Actor2D::GetPreFrameRelation(const Actor2D& target)const
{
	// �d�Ȃ��Ă���ꍇ0.0f��Ԃ�
	if(target.prePosition_.x - prePosition_.x == 0.0f && target.prePosition_.y - prePosition_.y == 0.0f){
		return 0.0f;
	}

	// ���0�Ƃ��Ď��v���Ɉʒu�֌W��Ԃ�
	return ideaMath::PI - atan2f(target.prePosition_.x - prePosition_.x, target.prePosition_.y - prePosition_.y);
}

//------------------------------------------------------------------------------
// �Ǐ]����^�[�Q�b�g�̐ݒ�
//------------------------------------------------------------------------------
void Actor2D::FollowActor(Actor2D& target)
{
	if(&target != this){	// �����͑ΏۂƂȂ肦�Ȃ�
		if(!target.pFollower_){	// �^�[�Q�b�g�Ƀt�H�����[�����݂��Ȃ����
			// ���ݎQ��
			pLeader_ = &target;
			target.pFollower_ = this;
		} else{
			// �^�[�Q�b�g�̃t�H�����[���^�[�Q�b�g�Ƃ��čēo�^
			FollowActor(*(target.pFollower_));
		}
	}
}

//------------------------------------------------------------------------------
// �t�H�����[����߂�
//------------------------------------------------------------------------------
void Actor2D::QuitFollower()
{
	if(pLeader_){	// ���[�_�[������̂Ȃ�
		if(pFollower_){	// �t�H�����[������̂Ȃ�
			// ���[�_�[�ƃt�H�����[���}�b�`������
			pLeader_->pFollower_ = pFollower_;
			pFollower_->pLeader_ = pLeader_;
		} else{
			pLeader_->pFollower_ = nullptr;
		}

		pFollower_ = nullptr;
		pLeader_ = nullptr;
	}
}

//------------------------------------------------------------------------------
// �t�H�����[�̈ʒu���ړ�
//------------------------------------------------------------------------------
void Actor2D::MoveFollower(Vector2D axis)
{
	// �ړ�
	position_ += axis;

	// ���̃t�H�����[�ւƂȂ�
	if(pFollower_){
		pFollower_->MoveFollower(axis);
	}
}

//------------------------------------------------------------------------------
// �t�H�����[�̉��U
//------------------------------------------------------------------------------
void Actor2D::ResetFollower(Actor2D* pTarget)
{
	if(pTarget){
		ResetFollower(pTarget->pFollower_);
		pTarget->QuitFollower();
	}
}

//------------------------------------------------------------------------------
// ���W�̕ێ�
//------------------------------------------------------------------------------
void Actor2D::UpdatePrePosition()
{
	// �O�t���[���̈ʒu��ۑ�
	prePosition_ = position_;
}
