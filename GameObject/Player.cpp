#include "Player.h"
#include "../Scene/Preview.h"
#include "../Scene/EditMode.h"
#include "Renderer.h"
#include "EditRenderer.h"
#include "Vita.h"
#include "Map.h"

using namespace std;
using namespace idea;

namespace{
	// �J�����̕����ɍ��킹���͂̌�����ς���
	Vector3D GetMoveAxis(Controller ctr, WrapCamera wcmr)
	{
		// ���͕���
		Vector2D inputAxis = ctr.GetLAxis();

		inputAxis += ctr.GetArrow();

		inputAxis = wcmr.FixVector(inputAxis.Normalized());

		return Vector3D(inputAxis.x, 0.0f, inputAxis.y);
	}
}

class Player::Impl{
public:
	// �O�t���[���̍��W
	Vector3D prePosition_;

	// �ړ��x�N�g��
	Vector3D moveVector_;

	// �̂�����x�N�g��
	Vector3D backVector_;

	// �Z�����W
	Cell cell_;

	// �R���g���[���[����
	Controller controller_;

	// ���f��
	PmxModel model_;

	// �ҋ@���[�V����
	VmdMotion motionIdol_;

	// �ړ����[�V����
	VmdMotion motionWalk_;

	// �R���C�_�[
	CapsuleMesh capsuleCollision_;

	// �ҋ@�J�E���g���̐����\���p�t�H���g
	BillboardFont oniCount_;

	// �A�C�e���擾���ʉ�
	Sound seDoping_;

	// ����vita
	Vita* targetVita_ = nullptr;

	// �X�^�~�i
	float stamina_ = 0.0f;

	// ��J����
	int exhausted_ = 0;

	// �A�C�e����������
	int doping_ = 0;

	// �f�o�b�O���
	Debug debugLog_;

	// �ҋ@���
	class Idol : public State<PlayerState>{
	public:
		Idol() = delete;
		Idol(Player& main) :
			State<PlayerState>(PlayerState::Idol),
			main_(main)
		{
		}

		// �X�V
		void Update() override;

	private:
		// �e
		Player& main_;
	};

	// �ړ����
	class Walk : public State<PlayerState>{
	public:
		Walk() = delete;
		Walk(Player& main) :
			State<PlayerState>(PlayerState::Walk),
			main_(main)
		{
		}

		// �X�V
		void Update() override;

	private:
		// �e
		Player& main_;
	};

	// �̂�������
	class Back : public State<PlayerState>{
	public:
		Back() = delete;
		Back(Player& main) :
			State<PlayerState>(PlayerState::Back),
			main_(main)
		{
		}

		// �X�V
		void Update() override;

		// �I������
		void UnInit() override;

	private:
		// �e
		Player& main_;
	};

	// �J�E���g���
	class Count : public State<PlayerState>{
	public:
		Count() = delete;
		Count(Player& main) :
			State<PlayerState>(PlayerState::Count),
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
		Player& main_;

		// �J�E���g����
		int count_ = 0;
	};
};

Player::Player() :
	pImpl_(new Player::Impl)
{
	Assert(pImpl_);
}

Player::~Player()
{
	SafeDelete(pImpl_);
}

void Player::Init()
{
	// ����
	auto& impl = (*pImpl_);

	// �V�[��
	auto& scene = (*pScene_);

	// �G�f�B�b�g���[�h�̃v���C���[�̓��f���̎�������
	if(typeid(scene) == typeid(EditMode)){
		// �����_���[
		auto& renderer = (*scene.GetGameObject<EditRenderer>(EDIT_RENDERER_LAYER));

		impl.model_.Init(position_);
		impl.model_.SetScale(0.7f, 0.7f, 0.7f);
		impl.model_.SetRotation(0.0f, DegreeToRadian(0), 0.0f);
		impl.model_.LoadPmxMeshFromStorage("asset/MODEL/Player/Player.pmx");
		renderer.AddObject(impl.model_);

		impl.motionIdol_.LoadVmdMotionFromStorage("asset/MOTION/_�ҋ@.vmd");

		return;
	}

	// �����_���[
	auto& renderer = (*scene.GetGameObject<Renderer>(RENDERER_LAYER));

	impl.prePosition_ = position_;

	impl.controller_.Init(1, true);
	impl.controller_.SetConfig(PadButton::A, KeyCode::A);
	impl.controller_.SetConfig(PadButton::B, KeyCode::S);
	impl.controller_.SetConfig(PadButton::R, KeyCode::Z);
	impl.controller_.SetConfig(PadButton::L, KeyCode::X);

	impl.model_.Init(position_);
	impl.model_.SetScale(0.2f, 0.2f, 0.2f);
	impl.model_.SetRotation(0.0f, DegreeToRadian(0), 0.0f);
	impl.model_.LoadPmxMeshFromStorage("asset/MODEL/Player/Player.pmx");
	renderer.AddObject(impl.model_);
	renderer.GetShadowCamera().AddObject(impl.model_);

	impl.motionIdol_.LoadVmdMotionFromStorage("asset/MOTION/_�ҋ@.vmd");
	impl.motionWalk_.LoadVmdMotionFromStorage("asset/MOTION/����.vmd");

	impl.capsuleCollision_.Init(position_.ReplaceY(position_.y + 2.5f));
	impl.capsuleCollision_.Create(0.5f, 1.0f);
	impl.capsuleCollision_.SetScale(5.0f, 5.0f, 5.0f);
	impl.capsuleCollision_.SetColor(ideaColor::CLEAR);
	renderer.AddObject(impl.capsuleCollision_);

	impl.oniCount_.Init(position_.ReplaceY(position_.y + 6.0f), renderer.GetWrapCamera(), 120);
	impl.oniCount_.DrawFormatText("3");
	impl.oniCount_.SetColor(ideaColor::CLEAR);

	AddState(make_shared<Player::Impl::Idol>(*this));
	AddState(make_shared<Player::Impl::Walk>(*this));
	AddState(make_shared<Player::Impl::Back>(*this));
	AddState(make_shared<Player::Impl::Count>(*this));

	SetState(PlayerState::Idol);

	impl.stamina_ = 300.0f;

	impl.exhausted_ = 0;
	impl.doping_ = 0;

	impl.seDoping_.LoadWaveFromStorage("asset/SOUND/pyuwaan000.wav");
	impl.seDoping_.SetVolume(0.6f);

	impl.targetVita_ = nullptr;
}

void Player::Update()
{
	// ����
	auto& impl = (*pImpl_);

	// �V�[��
	auto& scene = (*pScene_);

	// �G�f�B�b�g���[�h�Ȃ�~�܂��Ă��邾��
	if(typeid(scene) == typeid(EditMode)){
		impl.model_.SetPosition(position_);
		impl.model_.UpdateVmd(impl.motionIdol_, 1.0f, 0.1f, true, false);

		return;
	}

	// ��̏�艺��ňړ����x��ω�
	if((position_ - impl.prePosition_).y > 0.005f){
		position_ -= impl.moveVector_.Normalized() * 0.075f;
	} else if((position_ - impl.prePosition_).y < -0.2f){
		position_ += impl.moveVector_.Normalized() * 0.075f;
	}

	// �O�t���[���̍��W��ێ�
	impl.prePosition_ = position_;

	impl.controller_.Update();

	// �X�e�[�g�}�V���̍X�V
	StateMachine::Update();

	// �ړ�
	impl.moveVector_.y = Min(0.0f, impl.moveVector_.y);

	impl.moveVector_.y = impl.moveVector_.y - 0.2f;

	impl.moveVector_ *= 0.84f;

	impl.backVector_ *= 0.94f;

	position_ += impl.moveVector_ + impl.backVector_.ReplaceY(0);

	// �����Ⴍ�Ȃ�Ȃ��悤
	if(position_.y < 0.0f){
		position_.y = 0.0f;
		impl.moveVector_.y = 0.0f;
	}

	// �p�����[�^����
	if(impl.exhausted_){
		impl.exhausted_ = Max(0, impl.exhausted_ - 1);
		impl.stamina_ = Min(300.0f, impl.stamina_ + 300.0f / 210.0f);
	} else if(impl.doping_){
		impl.exhausted_ = 0;
		impl.doping_ = Max(0, impl.doping_ - 1);
		impl.stamina_ = 300.0f;
	} else{
		impl.stamina_ = Min(300.0f, impl.stamina_ + 0.25f);
	}

	// �v���r���[��vita�̏�Ԃ𑀂�
	if(typeid(scene) == typeid(Preview)){
		if(impl.controller_.GetButton(PadButton::A) == 1){
			auto vitas = scene.GetGameObjects<Vita>(VITA_LAYER);
			for(auto& vita : vitas){
				vita->Chase();
			}
		}

		if(impl.controller_.GetButton(PadButton::B) == 1){
			auto vitas = scene.GetGameObjects<Vita>(VITA_LAYER);
			for(auto& vita : vitas){
				vita->Idol();
			}
		}
	}

	// �J����
	auto& camera = scene.GetGameObject<Renderer>(RENDERER_LAYER)->GetWrapCamera();

	// �^�[�Q�b�g���b�N
	if(impl.controller_.GetButton(PadButton::L) == 1){
		LookOnVita();
	}

	if(impl.controller_.GetButton(PadButton::L)){
		// �^�[�Q�b�g�����ł�����؂�ւ���
		if(impl.targetVita_->GetState() == VitaState::Disappearance){
			LookOnVita();
		}

		// �^�[�Q�b�g��ǂ�
		if(impl.targetVita_){
			Vector3D vec = impl.targetVita_->GetPosition() - position_;
			if(vec.Normalized().LengthSquare() > 0.001f){
				Vector2D rot = { vec.ReplaceY(0).Normalized().x, vec.ReplaceY(0).Normalized().z };
				float rad = -(rot.Angle() + ideaMath::PIDIV2);
				camera.InterruptionWrap();
				camera.SetWrapTarget(ideaMath::PI * 1.0f - rad, DegreeToRadian(15));
			}
		}
	}

	camera.UpdateWrap(0.1f);

	impl.model_.SetPosition(position_);
	impl.model_.SetRotation(rotation_);

	impl.capsuleCollision_.SetPosition(position_.ReplaceY(position_.y + 2.5f));

	impl.oniCount_.SetPosition(position_.ReplaceY(position_.y + 6.0f));

	if(impl.debugLog_.GetDebugSwitch(0)){
		impl.capsuleCollision_.SetColor(ideaColor::TRANSLUCENT);
	} else{
		impl.capsuleCollision_.SetColor(ideaColor::CLEAR);
	}
}

void Player::Count()
{
	SetState(PlayerState::Count);
}

void Player::HitObstacle(const  Vector3D& axis, const  Vector3D& back)
{
	// ����
	auto& impl = (*pImpl_);

	impl.moveVector_.y = 0.0f;

	position_ += axis;

	if(back.LengthSquare() > 0.0f){
		impl.backVector_ = back;
		SetState(PlayerState::Back);
	}

	impl.model_.SetPosition(position_);

	impl.capsuleCollision_.SetPosition(position_.ReplaceY(position_.y + 2.5f));

	impl.oniCount_.SetPosition(position_.ReplaceY(position_.y + 6.0f));
}

void Player::Doping()
{
	// ����
	auto& impl = (*pImpl_);

	impl.doping_ = 90;
	impl.exhausted_ = 0;
	impl.seDoping_.Play();
}

void Player::SetCell(int x, int y)
{
	// ����
	auto& impl = (*pImpl_);

	impl.cell_.x = x;
	impl.cell_.y = y;
}

Cell& Player::GetCell()
{
	return pImpl_->cell_;
}

 CapsuleCollider Player::GetCollider()
{
	return pImpl_->capsuleCollision_.GetCollider();
}

float Player::GetStamina()
{
	return pImpl_->stamina_;
}

int Player::GetExhausted()
{
	return pImpl_->exhausted_;
}

int Player::GetDoping()
{
	return pImpl_->doping_;
}

void Player::LookOnVita()
{
	// ����
	auto& impl = (*pImpl_);

	// �V�[��
	auto& scene = (*pScene_);

	impl.targetVita_ = scene.GetGameObject<Vita>(VITA_LAYER);

	// vita���X�g
	auto vitas = scene.GetGameObjects<Vita>(VITA_LAYER);

	// vita�Ƃ̋���
	float distance = FLT_MAX;

	// �ł��߂�vita���^�[�Q�b�g�Ƃ���
	for(auto vita : vitas){
		// ���Œ���vita�͂͂���
		if(vita->GetState() == VitaState::Disappearance){
			continue;
		}

		if(Abs((vita->GetPosition() - position_).LengthSquare()) < distance){
			distance = Abs((vita->GetPosition() - position_).LengthSquare());
			impl.targetVita_ = vita;
		}
	}
}

void Player::Impl::Idol::Update()
{
	// ����
	auto& impl = (*main_.pImpl_);

	// �V�[��
	auto& scene = (*main_.pScene_);

	// �J����
	auto& camera = scene.GetGameObject<Renderer>(RENDERER_LAYER)->GetWrapCamera();

	// �ړ����Ă�����Walk�ֈڍs
	Vector3D move = GetMoveAxis(impl.controller_, camera);

	if(move.LengthSquare() > 0.0f){
		main_.SetState(PlayerState::Walk);
	}

	impl.model_.UpdateVmd(impl.motionIdol_, 2.5f, 0.1f, true, false);
}

void Player::Impl::Walk::Update()
{
	// ����
	auto& impl = (*main_.pImpl_);

	// �V�[��
	auto& scene = (*main_.pScene_);

	// �J����
	auto& camera = scene.GetGameObject<Renderer>(RENDERER_LAYER)->GetWrapCamera();

	// �ړ�
	Vector3D move = GetMoveAxis(impl.controller_, camera);

	float moveSpeed = 0.045f;

	if(move.LengthSquare() > 0.0f){
		// ����
		if(impl.doping_){
			moveSpeed += 0.03f;
		} else if(impl.exhausted_ == 0 && impl.stamina_ >= 0.9f && impl.controller_.GetButton(PadButton::R)){
			moveSpeed += 0.02f;

			// �v���r���[���̓X�^�~�i����Ȃ�
			if(typeid(scene) != typeid(Preview)){
				impl.stamina_ = Max(0.0f, impl.stamina_ - 1.0f);

				if(impl.stamina_ <= 0.0f){
					impl.exhausted_ = 210;
				}
			}
		}

		// ��J������
		if(impl.exhausted_){
			moveSpeed -= 0.015f;
		}

		impl.moveVector_ += move * moveSpeed;

		// ���f���̌�����ς���
		Vector2D rotationVector = { move.ReplaceY(0).Normalized().x, move.ReplaceY(0).Normalized().z };
		main_.rotation_.y = -(rotationVector.Angle() + ideaMath::PIDIV2);
	} else{
		main_.SetState(PlayerState::Idol);
	}

	impl.model_.UpdateVmd(impl.motionWalk_, moveSpeed * 30.0f);
}

void Player::Impl::Back::Update()
{
	// ����
	auto& impl = (*main_.pImpl_);

	// �̂�����؂�����Idol�ֈڍs
	if(impl.backVector_.LengthSquare() <= 0.01f){
		main_.SetState(PlayerState::Idol);
	}

	impl.model_.UpdateVmd(impl.motionIdol_, 1.0f, 0.1f, true, false);
}

void Player::Impl::Back::UnInit()
{
	// ����
	auto& impl = (*main_.pImpl_);

	impl.backVector_ = Vector3DZero();
}

void Player::Impl::Count::Init()
{
	// ����
	auto& impl = (*main_.pImpl_);

	// �J�E���g�͖�2.5�b
	count_ = 8 * 20;

	// �J�E���g�r���{�[�h�\��
	impl.oniCount_.SetColor(ideaColor::WHITE);
}

void Player::Impl::Count::Update()
{
	// ����
	auto& impl = (*main_.pImpl_);

	--count_;

	impl.oniCount_.DrawFormatText("%d", (count_ + 20) / 50);

	// �J�E���g���I�������Idol�ֈڍs
	if(count_ <= 0){
		main_.SetState(PlayerState::Idol);
	}

	impl.model_.UpdateVmd(impl.motionIdol_, 1.0f, 0.1f, true, false);
}

void Player::Impl::Count::UnInit()
{
	// ����
	auto& impl = (*main_.pImpl_);

	// �J�E���g�r���{�[�h��\��
	impl.oniCount_.SetColor(ideaColor::CLEAR);
}