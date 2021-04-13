#include "Vita.h"
#include "../Scene/MainGame.h"
#include "../Scene/EditMode.h"
#include "../Scene/Preview.h"
#include "Renderer.h"
#include "EditRenderer.h"
#include "Player.h"
#include "Map.h"
#include "MetaAI.h"

using namespace std;
using namespace idea;

class Vita::Impl{
public:
	// �O�t���[���̍��W
	Vector3D prePosition_;

	// �ړ��x�N�g��
	Vector3D moveVector_;

	// �̂�����x�N�g��
	Vector3D backVector_;

	// ����
	VitaRole role_ = VitaRole::Main;

	// �ǂ������鑬�x�̃��[�g
	float chaseSpeedRate_ = 0.0f;

	// �Z�����W
	Cell cell_;

	// �ŒZ�o�H���
	WayPointAStar wayPointAStar_;

	// ���f��
	PmxModel model_;

	// �ҋ@���[�V����
	VmdMotion motionIdol_;

	// �ړ����[�V����
	VmdMotion motionWalk_;

	// �R���C�_�[
	CapsuleMesh capsuleCollision_;

	// �o�ꎞ�̃G�t�F�N�g�\���p�r���{�[�h
	Billboard billboardAppearanceEffect_;

	// �o�ꎞ�̃G�t�F�N�g�̃e�N�X�`���[
	Texture textureAppearanceEffect_;

	// �ҋ@�J�E���g���̐����\���p�t�H���g
	BillboardFont oniCount_;

	// �f�o�b�O���
	Debug debugLog_;

	// �ҋ@���
	class Idol : public State<VitaState>{
	public:
		Idol() = delete;
		Idol(Vita& main) :
			State<VitaState>(VitaState::Idol),
			main_(main)
		{
		}

		// ������
		void Init() override;

		// �X�V
		void Update() override;

		// ��ԑJ��
		bool TransitionState();

	private:
		// �e
		Vita& main_;
	};

	// �߂Â����
	class StopBy : public State<VitaState>{
	public:
		StopBy() = delete;
		StopBy(Vita& main) :
			State<VitaState>(VitaState::StopBy),
			main_(main)
		{
		}

		// �X�V
		void Update() override;

		// ��ԑJ��
		bool TransitionState();

	private:
		// �e
		Vita& main_;
	};

	// �ǂ���������
	class Chase : public State<VitaState>{
	public:
		Chase() = delete;
		Chase(Vita& main) :
			State<VitaState>(VitaState::Chase),
			main_(main)
		{
		}

		// ������
		void Init() override;

		// �X�V
		void Update() override;

		// �I������
		void UnInit() override;

		// ��ԑJ��
		bool TransitionState();

		// �ړ�
		void Move();

	private:
		// �e
		Vita& main_;

		// �Ԋu��������t���O
		bool bSocial_ = false;

		// �Ԋu���J���Ă�������
		int socialCount_ = 0;
	};

	// ��������
	class Escape : public State<VitaState>{
	public:
		Escape() = delete;
		Escape(Vita& main) :
			State<VitaState>(VitaState::Escape),
			main_(main)
		{
		}

		// ������
		void Init() override;

		// �X�V
		void Update() override;

		// �I������
		void UnInit() override;

		// ��ԑJ��
		bool TransitionState();

		// �ړ�
		void Move();

		// ����������̎擾
		Vector3D GetEscapeDirection();

	private:
		// �e
		Vita& main_;

		// �ڕW���W
		Vector3D targetPosition_;

		// �ڕW�|�C���g
		Vector3D targetPoint_;
	};

	// ���E���
	class Withdrawal : public State<VitaState>{
	public:
		Withdrawal() = delete;
		Withdrawal(Vita& main) :
			State<VitaState>(VitaState::Withdrawal),
			main_(main)
		{
		}

		// ������
		void Init() override;

		// �X�V
		void Update() override;

		// �I������
		void UnInit() override;

		// ��ԑJ��
		bool TransitionState();

		// �ړ�
		void Move();

		// �ڕW�����̎擾
		Vector3D GetTargetDirection();

	private:
		// �e
		Vita& main_;

		// �ڕW���W
		Vector3D targetPosition_;

		// �ڕW�|�C���g
		Vector3D targetPoint_;
	};

	// �J�E���g���
	class Count : public State<VitaState>{
	public:
		Count() = delete;
		Count(Vita& main) :
			State<VitaState>(VitaState::Count),
			main_(main)
		{
		}

		// ������
		void Init() override;

		// �X�V
		void Update() override;

		// �I������
		void UnInit() override;

		// ��ԑJ��
		bool TransitionState();

	private:
		// �e
		Vita& main_;

		// �J�E���g����
		int count_ = 0;
	};

	// �o����
	class Appearance : public State<VitaState>{
	public:
		Appearance() = delete;
		Appearance(Vita& main) :
			State<VitaState>(VitaState::Appearance),
			main_(main)
		{
		}

		// ������
		void Init() override;

		// �X�V
		void Update() override;

		// �I������
		void UnInit() override;

		// ��ԑJ��
		bool TransitionState();

	private:
		// �e
		Vita& main_;

		// �J�E���g����
		int count_ = 0;
	};

	// ���ŏ��
	class Disappearance : public State<VitaState>{
	public:
		Disappearance() = delete;
		Disappearance(Vita& main) :
			State<VitaState>(VitaState::Disappearance),
			main_(main)
		{
		}

		// ������
		void Init() override;

		// �X�V
		void Update() override;

		// �I������
		void UnInit() override;

	private:
		// �e
		Vita& main_;

		// �J�E���g����
		int count_ = 0;
	};
};

Vita::Vita() :
	pImpl_(new Vita::Impl)
{
	Assert(pImpl_);
}

Vita::~Vita()
{
	SafeDelete(pImpl_);
}

void Vita::Init()
{
	// ����
	auto& impl = (*pImpl_);

	// �V�[��
	auto& scene = (*pScene_);

	// �G�f�B�b�g���[�h��vita�̓��f���̎�������
	if(typeid(scene) == typeid(EditMode)){
		// �����_���[
		auto& renderer = (*scene.GetGameObject<EditRenderer>(EDIT_RENDERER_LAYER));

		impl.model_.Init(position_);
		impl.model_.SetScale(0.7f, 0.7f, 0.7f);
		impl.model_.SetRotation(0.0f, DegreeToRadian(0), 0.0f);
		impl.model_.LoadPmxMeshFromStorage("asset/MODEL/Vita/Vita.pmx");
		renderer.AddObject(impl.model_);

		impl.motionIdol_.LoadVmdMotionFromStorage("asset/MOTION/_�ҋ@.vmd");

		return;
	}

	// �����_���[
	auto& renderer = (*scene.GetGameObject<Renderer>(RENDERER_LAYER));

	// �}�b�v���
	auto& map = (*scene.GetGameObject<Map>(MAP_LAYER));

	position_ = Vector3D(0, 0, 0);

	impl.prePosition_ = position_;

	impl.role_ = VitaRole::Main;

	impl.chaseSpeedRate_ = 8.0f;

	impl.model_.Init(position_);
	impl.model_.SetScale(0.2f, 0.2f, 0.2f);
	impl.model_.SetRotation(0.0f, DegreeToRadian(0), 0.0f);
	impl.model_.LoadPmxMeshFromStorage("asset/MODEL/Vita/Vita.pmx");
	impl.model_.SetColor(ideaColor::CLEAR);
	renderer.AddObject(impl.model_);

	impl.motionIdol_.LoadVmdMotionFromStorage("asset/MOTION/_�ҋ@.vmd");
	impl.motionWalk_.LoadVmdMotionFromStorage("asset/MOTION/����.vmd");

	impl.capsuleCollision_.Init(position_.ReplaceY(position_.y + 2.5f));
	impl.capsuleCollision_.Create(0.5f, 1.0f);
	impl.capsuleCollision_.SetScale(5.0f, 5.0f, 5.0f);
	impl.capsuleCollision_.SetColor(ideaColor::CLEAR);
	renderer.AddObject(impl.capsuleCollision_);

	impl.textureAppearanceEffect_.LoadImageFromStorage("asset/TEXTURE/bar02.bmp");

	impl.billboardAppearanceEffect_.Init(position_);
	impl.billboardAppearanceEffect_.SetScale(5.0f, 2000.0f, 0.0f);
	impl.billboardAppearanceEffect_.SetColor(ideaColor::CLEAR);
	impl.billboardAppearanceEffect_.SetBlend(1);
	impl.billboardAppearanceEffect_.SetYBillboardFlag(true);
	impl.billboardAppearanceEffect_.SetTexture(impl.textureAppearanceEffect_);
	renderer.GetWrapCamera().AddObject(impl.billboardAppearanceEffect_);

	impl.oniCount_.Init(position_.ReplaceY(position_.y + 6.0f), renderer.GetWrapCamera(), 120);
	impl.oniCount_.DrawFormatText("3");
	impl.oniCount_.SetColor(ideaColor::CLEAR);

	impl.wayPointAStar_.Init(map.GetWayPoints());

	AddState(make_shared<Vita::Impl::Idol>(*this));
	AddState(make_shared<Vita::Impl::StopBy>(*this));
	AddState(make_shared<Vita::Impl::Chase>(*this));
	AddState(make_shared<Vita::Impl::Escape>(*this));
	AddState(make_shared<Vita::Impl::Withdrawal>(*this));
	AddState(make_shared<Vita::Impl::Count>(*this));
	AddState(make_shared<Vita::Impl::Appearance>(*this));
	AddState(make_shared<Vita::Impl::Disappearance>(*this));

	// �v���r���[���ɃJ�E���g�͕s�v
	if(typeid(scene) == typeid(Preview)){
		SetState(VitaState::Chase);
	} else{
		SetState(VitaState::Count);
	}
}

void Vita::Update()
{
	// ����
	auto& impl = (*pImpl_);

	// �V�[��
	auto& scene = (*pScene_);

	impl.model_.SetColor(ideaColor::WHITE);

	// �G�f�B�b�g���[�h�Ȃ�~�܂��Ă��邾��
	if(typeid(scene) == typeid(EditMode)){
		impl.model_.SetPosition(position_);
		impl.model_.UpdateVmd(impl.motionIdol_, 1.0f, 0.1f, true, false);

		return;
	}

	// ���^AI���
	auto& metaAI = (*scene.GetGameObject<MetaAI>(META_AI_LAYER));

	// ��̏�艺��ňړ����x��ω�
	if((position_ - impl.prePosition_).y > 0.005f){
		position_ -= impl.moveVector_ * metaAI.GetVitaClimbRate();
	} else if((position_ - impl.prePosition_).y < -0.2f){
		position_ += impl.moveVector_ * 0.5f;
	}

	// �O�t���[���̍��W��ێ�
	impl.prePosition_ = position_;

	// �X�e�[�g�}�V���̍X�V
	StateMachine::Update();

	// �ړ�
	impl.moveVector_.y = Min(0.0f, impl.moveVector_.y);

	impl.moveVector_.y = impl.moveVector_.y - 0.2f;

	impl.moveVector_ *= 0.83f;

	impl.backVector_ *= 0.94f;

	position_ += impl.moveVector_ + impl.backVector_.ReplaceY(0);

	// �����Ⴍ�Ȃ�Ȃ��悤
	if(position_.y < 0.0f){
		position_.y = 0.0f;
		impl.moveVector_.y = 0.0f;
	}

	impl.model_.SetPosition(position_);
	impl.model_.SetRotation(rotation_);

	impl.capsuleCollision_.SetPosition(position_.ReplaceY(position_.y + 2.5f));

	impl.oniCount_.SetPosition(position_.ReplaceY(position_.y + 6.0f));

	// ��E�̕\��
	if(impl.debugLog_.IsDebugMode()
		&& GetState() != VitaState::Count
		&& GetState() != VitaState::Escape
		&& (typeid(scene) != typeid(Preview))){
		impl.oniCount_.SetColor(ideaColor::WHITE);
		switch(impl.role_){
			case VitaRole::Main:
				impl.oniCount_.DrawFormatText("C");

				break;

			case VitaRole::Chase:
				impl.oniCount_.DrawFormatText("C");

				break;

			case VitaRole::Wrap:
				impl.oniCount_.DrawFormatText("W");

				break;

			case VitaRole::Ambush:
				impl.oniCount_.DrawFormatText("A");

				break;
		}
	} else if(GetState() != VitaState::Escape
			  && GetState() != VitaState::Count){
		impl.oniCount_.SetColor(ideaColor::CLEAR);
	} else if(GetState() != VitaState::Count){
		impl.oniCount_.DrawFormatText("E");
	}

	if(impl.debugLog_.GetDebugSwitch(0)){
		impl.capsuleCollision_.SetColor(ideaColor::TRANSLUCENT);
	} else{
		impl.capsuleCollision_.SetColor(ideaColor::CLEAR);
	}
}

void Vita::HitObstacle(const  Vector3D& axis, const  Vector3D& back)
{
	// ����
	auto& impl = (*pImpl_);

	impl.moveVector_.y = 0.0f;

	if(back.LengthSquare() > 0.0f){
		impl.backVector_ = back;
	}

	position_ += axis;

	impl.model_.SetPosition(position_);

	impl.capsuleCollision_.SetPosition(position_.ReplaceY(position_.y + 2.5f));

	impl.oniCount_.SetPosition(position_.ReplaceY(position_.y + 6.0f));
}

void Vita::SetCell(int x, int y)
{
	// ����
	auto& impl = (*pImpl_);

	impl.cell_.x = x;
	impl.cell_.y = y;
}

 Cell& Vita::GetCell()
{
	return pImpl_->cell_;
}

 CapsuleCollider Vita::GetCollider()
{
	return pImpl_->capsuleCollision_.GetCollider();
}

void Vita::SetRole(VitaRole role)
{
	pImpl_->role_ = role;
}

VitaRole Vita::GetRole()
{
	return pImpl_->role_;
}

void Vita::Chase()
{
	SetState(VitaState::Chase);
}

void Vita::Idol()
{
	SetState(VitaState::Idol);
}

void Vita::Escape()
{
	SetState(VitaState::Escape);
}

void Vita::Count()
{
	SetState(VitaState::Count);
}

void Vita::Appearance()
{
	SetState(VitaState::Appearance);
}

void Vita::Disappearance()
{
	SetState(VitaState::Disappearance);
}

 Vector3D Vita::GetEscapePoint()
{
	// ����
	auto& impl = (*pImpl_);

	// �V�[��
	auto& scene = (*pScene_);

	// �v���C���[
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	return (position_ - player.GetPosition()).Normalized() * Vector3D(30.0f, 0.0f, 50.0f);
}

void Vita::SetChaseSpeedRate(float rate)
{
	pImpl_->chaseSpeedRate_ = rate;
}

void Vita::Impl::Idol::Init()
{
	// ����
	auto& impl = (*main_.pImpl_);

	// �ŒZ�o�H���
	auto& astar = impl.wayPointAStar_;

	astar.Reset();

	impl.moveVector_ = Vector3DZero();

	impl.model_.UpdateVmd(impl.motionIdol_, 2.5f, 0.1f, true, false);
}

void Vita::Impl::Idol::Update()
{
	// ����
	auto& impl = (*main_.pImpl_);

	// �V�[��
	auto& scene = (*main_.pScene_);

	// �v���C���[
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// ��ԑJ��
	if(TransitionState()){
		return;
	}

	// �����]��
	Vector3D direction = (player.GetPosition() - main_.position_).ReplaceY(0).Normalized();

	if(direction.LengthSquare() > 0.0f){
		// �����x�N�g��
		Vector2D rotationVector = { direction.ReplaceY(0).Normalized().x, direction.ReplaceY(0).Normalized().z };
		main_.rotation_.y = -(rotationVector.Angle() + ideaMath::PIDIV2);
	}

	impl.model_.UpdateVmd(impl.motionIdol_, 2.5f, 0.1f, true, false);
}

bool Vita::Impl::Idol::TransitionState()
{
	// ����
	auto& impl = (*main_.pImpl_);

	// �V�[��
	auto& scene = (*main_.pScene_);

	// �v���C���[
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// ���^AI���
	auto& metaAI = (*scene.GetGameObject<MetaAI>(META_AI_LAYER));

	if(main_.GetRole() == VitaRole::Main){
		// �{�̂��ҋ@���Ă���Ƃ��͓�����Ƃ�
		if((player.GetPosition() - main_.position_).ReplaceY(0).LengthSquare() < metaAI.GetVitaIdolRange() * metaAI.GetVitaIdolRange()){
			main_.SetState(VitaState::Escape);
			return true;
		}
	} else{
		// �{�̈ȊO�͕߂܂��ɍs��
		if((player.GetPosition() - main_.position_).ReplaceY(0).LengthSquare() < metaAI.GetVitaIdolRange() * metaAI.GetVitaIdolRange()){
			main_.SetState(VitaState::Chase);
			return true;
		}

		if((player.GetPosition() - main_.position_).ReplaceY(0).LengthSquare() < 8.0f * 8.0f
			&& (player.GetPosition().y - main_.position_.y) < 4.5f){
			main_.SetState(VitaState::StopBy);
			return true;
		}
	}

	return false;
}

void Vita::Impl::StopBy::Update()
{
	// ����
	auto& impl = (*main_.pImpl_);

	// �V�[��
	auto& scene = (*main_.pScene_);

	// �v���C���[
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// ���^AI���
	auto& metaAI = (*scene.GetGameObject<MetaAI>(META_AI_LAYER));

	if(TransitionState()){
		return;
	}

	// �Ǐ]����
	Vector3D moveDirection = (player.GetPosition() - main_.position_).ReplaceY(0).Normalized();

	// �ړ����x
	float moveSpeed = 0.0f;

	if(moveDirection.LengthSquare() > 0.0f){
		// �ړ�
		auto& metaAI = (*scene.GetGameObject<MetaAI>(META_AI_LAYER));

		moveSpeed = metaAI.GetVitaStopBySpeed();

		impl.moveVector_ += moveDirection * moveSpeed;

		// �����]��
		Vector2D rotationVector = { moveDirection.ReplaceY(0).Normalized().x, moveDirection.ReplaceY(0).Normalized().z };
		main_.rotation_.y = -(rotationVector.Angle() + ideaMath::PIDIV2);
	}

	if(moveSpeed > 0.0f){
		impl.model_.UpdateVmd(impl.motionWalk_, moveSpeed * 30.0f);
	} else{
		impl.model_.UpdateVmd(impl.motionIdol_, 2.5f, 0.1f, true, false);
	}
}

bool Vita::Impl::StopBy::TransitionState()
{
	// ����
	auto& impl = (*main_.pImpl_);

	// �V�[��
	auto& scene = (*main_.pScene_);

	// �v���C���[
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// �v���C���[�Ɨ���Ă��܂�����
	if((player.GetPosition() - main_.position_).ReplaceY(0).LengthSquare() > 10.0f * 10.0f
		|| (player.GetPosition().y - main_.position_.y) > 4.5f){
		switch(impl.role_){
			case VitaRole::Main:
			case VitaRole::Chase:
				main_.SetState(VitaState::Chase);
				return true;
				break;

			case VitaRole::Wrap:
			case VitaRole::Ambush:
				main_.SetState(VitaState::Withdrawal);
				return true;
				break;
		}
	}

	return false;
}

void Vita::Impl::Chase::Init()
{
	bSocial_ = false;

	socialCount_ = 0;
}

void Vita::Impl::Chase::Update()
{
	// ����
	auto& impl = (*main_.pImpl_);

	// �V�[��
	auto& scene = (*main_.pScene_);

	// �v���C���[
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// vita�{��
	auto& vita = (*scene.GetGameObject<Vita>(VITA_LAYER));

	// �ŒZ�o�H���
	auto& astar = impl.wayPointAStar_;

	if(impl.role_ == VitaRole::Wrap
		|| impl.role_ == VitaRole::Ambush){
		// �{�̂Ƌ�����u��
		if((vita.GetPosition() - main_.position_).LengthSquare() < 20.0f * 20.0f
			&& (player.GetPosition() - main_.position_).ReplaceY(0).LengthSquare() > 20.0f * 0.0f){
			++socialCount_;
		}
	}

	if(TransitionState()){
		return;
	}

	if(impl.backVector_.LengthSquare() > 0.0f){
		astar.Reset();
	}

	Move();
}

void Vita::Impl::Chase::UnInit()
{
	// ����
	auto& impl = (*main_.pImpl_);

	// �ŒZ�o�H���
	auto& astar = impl.wayPointAStar_;

	astar.Reset();
}

bool Vita::Impl::Chase::TransitionState()
{
	// ����
	auto& impl = (*main_.pImpl_);

	// �V�[��
	auto& scene = (*main_.pScene_);

	// �v���C���[
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// vita�{��
	auto& vita = (*scene.GetGameObject<Vita>(VITA_LAYER));

	// ���^AI���
	auto& metaAI = (*scene.GetGameObject<MetaAI>(META_AI_LAYER));

	// �ŒZ�o�H���
	auto& astar = impl.wayPointAStar_;

	// �J����
	auto& camera = scene.GetGameObject<Renderer>(RENDERER_LAYER)->GetWrapCamera();

	switch(impl.role_){
		case VitaRole::Main:
		case VitaRole::Chase:
			// �v���C���[�Ƌ������߂���ΐڋ�
			if((player.GetPosition() - main_.position_).ReplaceY(0).LengthSquare() < 8.0f * 8.0f
				&& (player.GetPosition().y - main_.position_.y) < 4.5f
				&& astar.IsGoal()){
				main_.SetState(VitaState::StopBy);
				return true;
			}
			break;

		case VitaRole::Wrap:
		case VitaRole::Ambush:
			// �v���C���[�Ƌ�����������Η��E
			if((player.GetPosition() - main_.position_).ReplaceY(0).LengthSquare() > 25.0f * 25.0f
				&& socialCount_ > 5 * 60){
				main_.SetState(VitaState::Withdrawal);
				return true;
			}

			// �J�����Ɏʂ��Ă��Ȃ���Η��E
			if(!camera.IsVisible(impl.capsuleCollision_)){
				main_.SetState(VitaState::Withdrawal);
				return true;
			}

			// �v���C���[�Ƌ������߂���ΐڋ�
			if((player.GetPosition() - main_.position_).ReplaceY(0).LengthSquare() < 8.0f * 8.0f
				&& (player.GetPosition().y - main_.position_.y) < 4.5f
				&& astar.IsGoal()){
				main_.SetState(VitaState::StopBy);
				return true;
			}
			break;
	}

	return false;
}

void Vita::Impl::Chase::Move()
{
	// ����
	auto& impl = (*main_.pImpl_);

	// �V�[��
	auto& scene = (*main_.pScene_);

	// �v���C���[
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// vita�{��
	auto& vita = (*scene.GetGameObject<Vita>(VITA_LAYER));

	// ���^AI���
	auto& metaAI = (*scene.GetGameObject<MetaAI>(META_AI_LAYER));

	// �ŒZ�o�H���
	auto& astar = impl.wayPointAStar_;

	// �ڕW���W
	Vector3D targetPosition = astar.GetNextPosition(main_.position_, player.GetPosition().ReplaceY(player.GetPosition().y + 3.5f), 0.2f).ReplaceY(0);

	// �ړ�����
	Vector3D moveDirection = (targetPosition - main_.position_).ReplaceY(0).Normalized();

	// �{�̈ȊO��
	if(impl.role_ != VitaRole::Main){
		// vita�{�̂ɋ߂���Η����悤�ݒ�
		if(!bSocial_){
			if(!bSocial_ && (vita.GetPosition() - main_.position_).LengthSquare() < 8.0f * 8.0f){
				moveDirection = (main_.position_ - vita.GetPosition()).ReplaceY(0).Normalized();
				bSocial_ = true;
			}
		} else{
			moveDirection = (main_.position_ - vita.GetPosition()).ReplaceY(0).Normalized();

			if((vita.GetPosition() - main_.position_).LengthSquare() > 10.0f * 10.0f){
				astar.Reset();
				targetPosition = astar.GetNextPosition(main_.position_, player.GetPosition().ReplaceY(player.GetPosition().y + 3.5f), 0.2f).ReplaceY(0);
				moveDirection = (targetPosition - main_.position_).ReplaceY(0).Normalized();
				bSocial_ = false;
			}
		}
	}

	// �ړ����x
	float moveSpeed = 0.0f;

	if(moveDirection.LengthSquare() > 0.0f){
		// �X�s�[�h����
		moveSpeed = metaAI.GetVitaChaseSpeed();
		if(impl.role_ != VitaRole::Main){
			moveSpeed = metaAI.GetVitaChaseSpeed() * impl.chaseSpeedRate_;
		}

		impl.moveVector_ += moveDirection * moveSpeed;

		// �����]��
		Vector2D rotationVector = { moveDirection.ReplaceY(0).Normalized().x, moveDirection.ReplaceY(0).Normalized().z };

		main_.rotation_.y = -(rotationVector.Angle() + ideaMath::PIDIV2);
	}

	if(moveSpeed > 0.0f){
		impl.model_.UpdateVmd(impl.motionWalk_, moveSpeed * 30.0f);
	} else{
		impl.model_.UpdateVmd(impl.motionIdol_, 2.5f, 0.1f, true, false);
	}
}

void Vita::Impl::Escape::Init()
{
	// ������̕���
	Vector3D escapeDirection = GetEscapeDirection();

	targetPoint_ = (escapeDirection).Normalized() * Vector3D(30.0f, 0.0f, 50.0f);

	// ���ݒn�Ƌ߂���Ώ�����ς���
	if((main_.position_ - targetPoint_).LengthSquare() < 15.0f * 15.0f){
		targetPoint_ = (escapeDirection).Normalized() * Vector3D(50.0f, 0.0f, 30.0f);
	}
}

void Vita::Impl::Escape::Update()
{
	if(TransitionState()){
		return;
	}

	Move();
}

void Vita::Impl::Escape::UnInit()
{
	// ����
	auto& impl = (*main_.pImpl_);

	// �ŒZ�o�H���
	auto& astar = impl.wayPointAStar_;

	astar.Reset();
}

bool Vita::Impl::Escape::TransitionState()
{
	// ����
	auto& impl = (*main_.pImpl_);

	// �{�̈ȊO�������Ă���̂͂��������̂ŋ�������
	if(impl.role_ != VitaRole::Main){
		main_.SetState(VitaState::Disappearance);
		return true;
	}

	return false;
}

void Vita::Impl::Escape::Move()
{
	// ����
	auto& impl = (*main_.pImpl_);

	// �V�[��
	auto& scene = (*main_.pScene_);

	// �v���C���[
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// ���^AI���
	auto& metaAI = (*scene.GetGameObject<MetaAI>(META_AI_LAYER));

	// �ŒZ�o�H���
	auto& astar = impl.wayPointAStar_;

	if(astar.IsGoal()){
		// �ҋ@�͈͂��L����Γ�����
		if((main_.position_ - player.GetPosition()).ReplaceY(0).LengthSquare() < metaAI.GetVitaIdolRange() * metaAI.GetVitaIdolRange()){
			// ������̕���
			Vector3D escapeDirection = GetEscapeDirection();

			targetPoint_ = (escapeDirection).Normalized() * Vector3D(30.0f, 0.0f, 50.0f);

			// ���ݒn�Ƌ߂���Ώ�����ς���
			if((main_.position_ - targetPoint_).LengthSquare() < 15.0f * 15.0f){
				targetPoint_ = (escapeDirection).Normalized() * Vector3D(50.0f, 0.0f, 30.0f);
			}
		} else{
			// �ҋ@��ԂɑJ��
			main_.SetState(VitaState::Idol);

			return;
		}
	}

	targetPosition_ = astar.GetNextPosition(main_.position_, targetPoint_, 0.2f).ReplaceY(0);

	// �ړ�����
	Vector3D moveDirection = (targetPosition_ - main_.position_).ReplaceY(0).Normalized();

	// �ړ��ł��Ȃ�������
	if(moveDirection == Vector3DZero()){
		// �ڕW�g�僌�[�g
		float rate = 1.0f;

		// ����
		int code = 1;

		// ������̕���
		Vector3D escapeDirection = GetEscapeDirection();

		while(moveDirection == Vector3DZero() && code){
			rate += 0.1f;

			targetPoint_ = (escapeDirection).Normalized() * Vector3D(15.0f, 0.0f, 25.0f) * rate * (float)code;

			if((main_.position_ - targetPoint_).LengthSquare() < 15.0f * 15.0f){
				targetPoint_ = (escapeDirection).Normalized() * Vector3D(25.0f, 0.0f, 15.0f) * rate * (float)code;
			}

			targetPosition_ = astar.GetNextPosition(main_.position_, targetPoint_, 0.2f).ReplaceY(0);


			moveDirection = (targetPosition_ - main_.position_).ReplaceY(0).Normalized();

			if(rate > 200.0f){
				rate = 1.0f;

				if(code == 1){
					code = -1;
				} else{
					code = 0;
				}
			}
		}
	}

	// �ړ����x
	float moveSpeed = 0.0f;

	if(moveDirection.LengthSquare() > 0.0f){
		moveSpeed = metaAI.GetVitaEscapeSpeed();

		impl.moveVector_ += moveDirection * moveSpeed;

		// �����]��
		Vector2D rotationVector = { moveDirection.ReplaceY(0).Normalized().x, moveDirection.ReplaceY(0).Normalized().z };
		main_.rotation_.y = -(rotationVector.Angle() + ideaMath::PIDIV2);
	} else{
		main_.SetState(VitaState::Idol);

		return;
	}

	if(moveSpeed > 0.0f){
		impl.model_.UpdateVmd(impl.motionWalk_, moveSpeed * 30.0f);
	}
}

Vector3D Vita::Impl::Escape::GetEscapeDirection()
{
	// �V�[��
	auto& scene = (*main_.pScene_);

	// �v���C���[
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// ������̕���
	Vector3D escapeDirection = (main_.position_ - player.GetPosition()).ReplaceY(0).Normalized();

	// ���������Ȃ���Έړ��ł���悤�ɕ␳
	if(escapeDirection.x == 0.0f){
		escapeDirection.x = 0.5f;
		escapeDirection.z = 0.5f * Sign(escapeDirection.z);
	} else if(Abs(escapeDirection.x) < 0.5f){
		escapeDirection.x = 0.5f * Sign(main_.position_.x);
		escapeDirection.z = 0.5f * Sign(escapeDirection.z);
	}

	if(escapeDirection.z == 0.0f){
		escapeDirection.x = 0.5f * Sign(escapeDirection.x);
		escapeDirection.z = 0.5f;
	} else if(Abs(escapeDirection.z) < 0.5f){
		escapeDirection.x = 0.5f * Sign(escapeDirection.x);
		escapeDirection.z = 0.5f * Sign(main_.position_.z);
	}

	return escapeDirection;
}

void Vita::Impl::Withdrawal::Init()
{
	// �ڕW�̕���
	Vector3D targetDirection = GetTargetDirection();

	targetPoint_ = (targetDirection).Normalized() * Vector3D(30.0f, 0.0f, 50.0f);

	if((main_.position_ - targetPoint_).LengthSquare() < 15.0f * 15.0f){
		targetPoint_ = (targetDirection).Normalized() * Vector3D(50.0f, 0.0f, 30.0f);
	}
}

void Vita::Impl::Withdrawal::Update()
{
	if(TransitionState()){
		return;
	}

	Move();
}

void Vita::Impl::Withdrawal::UnInit()
{
	// ����
	auto& impl = (*main_.pImpl_);

	// �ŒZ�o�H���
	auto& astar = impl.wayPointAStar_;

	astar.Reset();
}

bool Vita::Impl::Withdrawal::TransitionState()
{
	// ����
	auto& impl = (*main_.pImpl_);

	// �V�[��
	auto& scene = (*main_.pScene_);

	// �v���C���[
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// �ŒZ�o�H���
	auto& astar = impl.wayPointAStar_;

	if((player.GetPosition() - main_.position_).ReplaceY(0).LengthSquare() < 8.0f * 8.0f
		&& (player.GetPosition().y - main_.position_.y) < 4.5f){
		main_.SetState(VitaState::StopBy);

		return true;
	}

	if(!astar.IsGoal()){
		return false;
	}

	if(impl.role_ == VitaRole::Ambush){
		main_.SetState(VitaState::Idol);

		return true;
	} else{
		main_.SetState(VitaState::Chase);

		return true;
	}

	return false;
}

void Vita::Impl::Withdrawal::Move()
{
	// ����
	auto& impl = (*main_.pImpl_);

	// �V�[��
	auto& scene = (*main_.pScene_);

	// �v���C���[
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// vita���X�g
	auto vitas = scene.GetGameObjects<Vita>(VITA_LAYER);

	// ���^AI���
	auto& metaAI = (*scene.GetGameObject<MetaAI>(META_AI_LAYER));

	// �ŒZ�o�H���
	auto& astar = impl.wayPointAStar_;

	// �ق���vita�Ƌ��������Z���ɂ��邩
	bool isConflict = false;

	for(auto& vita : vitas){
		if(&main_ == vita){
			continue;
		}

		if(main_.GetCell() == vita->GetCell()){
			isConflict = true;

			break;
		}
	}

	if(isConflict){
		// �ڕW�̕���
		Vector3D targetDirection = GetTargetDirection();

		targetPoint_ = (targetDirection).Normalized() * Vector3D(30.0f, 0.0f, 50.0f);

		if((main_.position_ - targetPoint_).LengthSquare() < 15.0f * 15.0f){
			targetPoint_ = (targetDirection).Normalized() * Vector3D(50.0f, 0.0f, 30.0f);
		}
	}

	targetPosition_ = astar.GetNextPosition(main_.position_, targetPoint_, 0.2f).ReplaceY(0);

	// �ړ�����
	Vector3D moveDirection = (targetPosition_ - main_.position_).ReplaceY(0).Normalized();

	// �ړ��ł��Ȃ�������
	if(moveDirection == Vector3DZero()){
		// �ڕW�g�僌�[�g
		float rate = 1.0f;

		// ����
		int code = 1;

		// ������̕���
		Vector3D targetDirection = GetTargetDirection();

		while(moveDirection == Vector3DZero() && code){
			rate += 0.1f;

			targetPoint_ = (targetDirection).Normalized() * Vector3D(10.0f, 0.0f, 15.0f) * rate * (float)code;

			if((main_.position_ - targetPoint_).LengthSquare() < 15.0f * 15.0f){
				targetPoint_ = (targetDirection).Normalized() * Vector3D(15.0f, 0.0f, 10.0f) * rate * (float)code;
			}

			targetPosition_ = astar.GetNextPosition(main_.position_, targetPoint_, 0.2f).ReplaceY(0);

			moveDirection = (targetPosition_ - main_.position_).ReplaceY(0).Normalized();

			if(rate > 200.0f){
				rate = 1.0f;

				if(code == 1){
					code = -1;
				} else{
					code = 0;
				}
			}
		}
	}

	// �ړ����x
	float moveSpeed = 0.0f;

	if(moveDirection.LengthSquare() > 0.0f){
		moveSpeed = metaAI.GetVitaEscapeSpeed();

		impl.moveVector_ += moveDirection * moveSpeed;

		// �����]��
		Vector2D rotationVector = { moveDirection.ReplaceY(0).Normalized().x, moveDirection.ReplaceY(0).Normalized().z };
		main_.rotation_.y = -(rotationVector.Angle() + ideaMath::PIDIV2);
	}

	if(moveSpeed > 0.0f){
		impl.model_.UpdateVmd(impl.motionWalk_, moveSpeed * 30.0f);
	} else{
		impl.model_.UpdateVmd(impl.motionIdol_, 2.5f, 0.1f, true, false);
	}
}

Vector3D Vita::Impl::Withdrawal::GetTargetDirection()
{
	// �V�[��
	auto& scene = (*main_.pScene_);

	// �v���C���[
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// vita�{��
	auto& vita = (*scene.GetGameObject<Vita>(VITA_LAYER));

	// �ڕW�̕���
	Vector3D targetDirection = (player.GetPosition() - vita.GetPosition()).ReplaceY(0).Normalized();

	// ���������Ȃ���Έړ��ł���悤�ɕ␳
	if(targetDirection.x == 0.0f){
		targetDirection.x = 0.5f;
		targetDirection.z = 0.5f * Sign(targetDirection.z);
	} else if(Abs(targetDirection.x) < 0.25f){
		targetDirection.x = 0.5f * Sign(main_.position_.x);
		targetDirection.z = 0.5f * Sign(targetDirection.z);
	}

	if(targetDirection.z == 0.0f){
		targetDirection.x = 0.5f * Sign(targetDirection.x);
		targetDirection.z = 0.5f;
	} else if(Abs(targetDirection.z) < 0.25f){
		targetDirection.x = 0.5f * Sign(targetDirection.x);
		targetDirection.z = 0.5f * Sign(main_.position_.z);
	}

	return targetDirection;
}

void Vita::Impl::Count::Init()
{
	// ����
	auto& impl = (*main_.pImpl_);

	count_ = 8 * 20;

	impl.oniCount_.SetColor(ideaColor::CLEAR);
}

void Vita::Impl::Count::Update()
{
	// ����
	auto& impl = (*main_.pImpl_);

	impl.oniCount_.SetColor(ideaColor::WHITE);

	--count_;

	impl.oniCount_.DrawFormatText("%d", (count_ + 20) / 50);

	if(count_ <= 0){
		if(TransitionState()){
			return;
		}
	}

	impl.model_.UpdateVmd(impl.motionIdol_, 1.0f, 0.1f, true, false);
}

void Vita::Impl::Count::UnInit()
{
	// ����
	auto& impl = (*main_.pImpl_);

	impl.oniCount_.SetColor(ideaColor::CLEAR);
}

bool Vita::Impl::Count::TransitionState()
{
	main_.SetState(VitaState::Chase);

	return false;
}

void Vita::Impl::Appearance::Init()
{
	// ����
	auto& impl = (*main_.pImpl_);

	// �V�[��
	auto& scene = (*main_.pScene_);

	// �����_���[
	auto& renderer = (*scene.GetGameObject<Renderer>(RENDERER_LAYER));

	count_ = 0;

	impl.billboardAppearanceEffect_.SetScale(5.0f, 2000.0f, 0.0f);
	impl.billboardAppearanceEffect_.SetColor(ideaColor::CLEAR);

	impl.moveVector_ = Vector3DZero();

	impl.model_.UpdateVmd(impl.motionIdol_, 0.0f, 1.0f, true, false);
}

void Vita::Impl::Appearance::Update()
{
	// ����
	auto& impl = (*main_.pImpl_);

	impl.billboardAppearanceEffect_.SetPosition(main_.position_.ReplaceY(main_.position_.y + 1000.0f));

	if(count_ < 60){
		impl.model_.SetColor(ideaColor::CLEAR);
	} else{
		impl.model_.SetColor(ideaColor::WHITE.ReplaceAlpha((count_) / 120.0f));
	}

	if(count_ > 150){
		// �G�t�F�N�g�̕�
		float effectWidth = (1.0f - ((count_ - 150) / 30.0f)) * 5.0f;
		impl.billboardAppearanceEffect_.SetScale(effectWidth, 2000.0f, 0.0f);
	} else{
		impl.billboardAppearanceEffect_.SetColor(ideaColor::SOFT_BLUE.ReplaceAlpha((count_ / 180.0f)));
	}

	++count_;

	if(count_ > 180){
		impl.billboardAppearanceEffect_.SetColor(ideaColor::CLEAR);
		TransitionState();
		return;
	}
}

void Vita::Impl::Appearance::UnInit()
{
	// ����
	auto& impl = (*main_.pImpl_);

	impl.billboardAppearanceEffect_.SetColor(ideaColor::CLEAR);
}

bool Vita::Impl::Appearance::TransitionState()
{
	// ����
	auto& impl = (*main_.pImpl_);

	switch(impl.role_){
		case VitaRole::Main:
		case VitaRole::Chase:
			main_.SetState(VitaState::Chase);

			return true;

		case VitaRole::Wrap:
		case VitaRole::Ambush:
			main_.SetState(VitaState::Withdrawal);

			return true;
	}

	return false;
}

void Vita::Impl::Disappearance::Init()
{
	// ����
	auto& impl = (*main_.pImpl_);

	count_ = 60;

	impl.billboardAppearanceEffect_.SetScale(0.0f, 2000.0f, 0.0f);
	impl.billboardAppearanceEffect_.SetColor(ideaColor::SOFT_BLUE.ReplaceAlpha(0.8f));

	impl.moveVector_ = Vector3DZero();

	impl.model_.UpdateVmd(impl.motionIdol_, 0.0f, 1.0f, true, false);
}

void Vita::Impl::Disappearance::Update()
{
	// ����
	auto& impl = (*main_.pImpl_);

	impl.model_.SetColor(ideaColor::WHITE.ReplaceAlpha(count_ / 60.0f));

	impl.billboardAppearanceEffect_.SetPosition(main_.position_.ReplaceY(main_.position_.y + 1000.0f));

	// �G�t�F�N�g�̕�
	float effectWidth = (((count_) / 60.0f)) * 10.0f;

	impl.billboardAppearanceEffect_.SetScale(effectWidth, 2000.0f, 0.0f);
	impl.billboardAppearanceEffect_.SetColor(ideaColor::SOFT_BLUE.ReplaceAlpha(1.0f - (count_ / 60.0f) - 0.2f));

	--count_;

	if(!count_){
		main_.SetDestroy();
	}
}

void Vita::Impl::Disappearance::UnInit()
{
	// ����
	auto& impl = (*main_.pImpl_);

	impl.billboardAppearanceEffect_.SetColor(ideaColor::CLEAR);
}