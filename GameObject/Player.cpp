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
	// カメラの方向に合わせ入力の向きを変える
	Vector3D GetMoveAxis(Controller ctr, WrapCamera wcmr)
	{
		// 入力方向
		Vector2D inputAxis = ctr.GetLAxis();

		inputAxis += ctr.GetArrow();

		inputAxis = wcmr.FixVector(inputAxis.Normalized());

		return Vector3D(inputAxis.x, 0.0f, inputAxis.y);
	}
}

class Player::Impl{
public:
	// 前フレームの座標
	Vector3D prePosition_;

	// 移動ベクトル
	Vector3D moveVector_;

	// のけぞりベクトル
	Vector3D backVector_;

	// セル座標
	Cell cell_;

	// コントローラー入力
	Controller controller_;

	// モデル
	PmxModel model_;

	// 待機モーション
	VmdMotion motionIdol_;

	// 移動モーション
	VmdMotion motionWalk_;

	// コライダー
	CapsuleMesh capsuleCollision_;

	// 待機カウント時の数字表示用フォント
	BillboardFont oniCount_;

	// アイテム取得効果音
	Sound seDoping_;

	// 注視vita
	Vita* targetVita_ = nullptr;

	// スタミナ
	float stamina_ = 0.0f;

	// 疲労時間
	int exhausted_ = 0;

	// アイテム加速時間
	int doping_ = 0;

	// デバッグ情報
	Debug debugLog_;

	// 待機状態
	class Idol : public State<PlayerState>{
	public:
		Idol() = delete;
		Idol(Player& main) :
			State<PlayerState>(PlayerState::Idol),
			main_(main)
		{
		}

		// 更新
		void Update() override;

	private:
		// 親
		Player& main_;
	};

	// 移動状態
	class Walk : public State<PlayerState>{
	public:
		Walk() = delete;
		Walk(Player& main) :
			State<PlayerState>(PlayerState::Walk),
			main_(main)
		{
		}

		// 更新
		void Update() override;

	private:
		// 親
		Player& main_;
	};

	// のけぞり状態
	class Back : public State<PlayerState>{
	public:
		Back() = delete;
		Back(Player& main) :
			State<PlayerState>(PlayerState::Back),
			main_(main)
		{
		}

		// 更新
		void Update() override;

		// 終了処理
		void UnInit() override;

	private:
		// 親
		Player& main_;
	};

	// カウント状態
	class Count : public State<PlayerState>{
	public:
		Count() = delete;
		Count(Player& main) :
			State<PlayerState>(PlayerState::Count),
			main_(main)
		{
		}

		// 初期化
		void Init() override;

		// 更新
		void Update() override;

		// 終了処理
		void UnInit() override;

	private:
		// 親
		Player& main_;

		// カウント時間
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
	// 実装
	auto& impl = (*pImpl_);

	// シーン
	auto& scene = (*pScene_);

	// エディットモードのプレイヤーはモデルの実装だけ
	if(typeid(scene) == typeid(EditMode)){
		// レンダラー
		auto& renderer = (*scene.GetGameObject<EditRenderer>(EDIT_RENDERER_LAYER));

		impl.model_.Init(position_);
		impl.model_.SetScale(0.7f, 0.7f, 0.7f);
		impl.model_.SetRotation(0.0f, DegreeToRadian(0), 0.0f);
		impl.model_.LoadPmxMeshFromStorage("asset/MODEL/Player/Player.pmx");
		renderer.AddObject(impl.model_);

		impl.motionIdol_.LoadVmdMotionFromStorage("asset/MOTION/_待機.vmd");

		return;
	}

	// レンダラー
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

	impl.motionIdol_.LoadVmdMotionFromStorage("asset/MOTION/_待機.vmd");
	impl.motionWalk_.LoadVmdMotionFromStorage("asset/MOTION/歩く.vmd");

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
	// 実装
	auto& impl = (*pImpl_);

	// シーン
	auto& scene = (*pScene_);

	// エディットモードなら止まっているだけ
	if(typeid(scene) == typeid(EditMode)){
		impl.model_.SetPosition(position_);
		impl.model_.UpdateVmd(impl.motionIdol_, 1.0f, 0.1f, true, false);

		return;
	}

	// 坂の上り下りで移動速度を変化
	if((position_ - impl.prePosition_).y > 0.005f){
		position_ -= impl.moveVector_.Normalized() * 0.075f;
	} else if((position_ - impl.prePosition_).y < -0.2f){
		position_ += impl.moveVector_.Normalized() * 0.075f;
	}

	// 前フレームの座標を保持
	impl.prePosition_ = position_;

	impl.controller_.Update();

	// ステートマシンの更新
	StateMachine::Update();

	// 移動
	impl.moveVector_.y = Min(0.0f, impl.moveVector_.y);

	impl.moveVector_.y = impl.moveVector_.y - 0.2f;

	impl.moveVector_ *= 0.84f;

	impl.backVector_ *= 0.94f;

	position_ += impl.moveVector_ + impl.backVector_.ReplaceY(0);

	// 床より低くならないよう
	if(position_.y < 0.0f){
		position_.y = 0.0f;
		impl.moveVector_.y = 0.0f;
	}

	// パラメータ増減
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

	// プレビュー時vitaの状態を操る
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

	// カメラ
	auto& camera = scene.GetGameObject<Renderer>(RENDERER_LAYER)->GetWrapCamera();

	// ターゲットロック
	if(impl.controller_.GetButton(PadButton::L) == 1){
		LookOnVita();
	}

	if(impl.controller_.GetButton(PadButton::L)){
		// ターゲットが消滅したら切り替える
		if(impl.targetVita_->GetState() == VitaState::Disappearance){
			LookOnVita();
		}

		// ターゲットを追う
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
	// 実装
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
	// 実装
	auto& impl = (*pImpl_);

	impl.doping_ = 90;
	impl.exhausted_ = 0;
	impl.seDoping_.Play();
}

void Player::SetCell(int x, int y)
{
	// 実装
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
	// 実装
	auto& impl = (*pImpl_);

	// シーン
	auto& scene = (*pScene_);

	impl.targetVita_ = scene.GetGameObject<Vita>(VITA_LAYER);

	// vitaリスト
	auto vitas = scene.GetGameObjects<Vita>(VITA_LAYER);

	// vitaとの距離
	float distance = FLT_MAX;

	// 最も近いvitaをターゲットとする
	for(auto vita : vitas){
		// 消滅中のvitaははじく
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
	// 実装
	auto& impl = (*main_.pImpl_);

	// シーン
	auto& scene = (*main_.pScene_);

	// カメラ
	auto& camera = scene.GetGameObject<Renderer>(RENDERER_LAYER)->GetWrapCamera();

	// 移動していたらWalkへ移行
	Vector3D move = GetMoveAxis(impl.controller_, camera);

	if(move.LengthSquare() > 0.0f){
		main_.SetState(PlayerState::Walk);
	}

	impl.model_.UpdateVmd(impl.motionIdol_, 2.5f, 0.1f, true, false);
}

void Player::Impl::Walk::Update()
{
	// 実装
	auto& impl = (*main_.pImpl_);

	// シーン
	auto& scene = (*main_.pScene_);

	// カメラ
	auto& camera = scene.GetGameObject<Renderer>(RENDERER_LAYER)->GetWrapCamera();

	// 移動
	Vector3D move = GetMoveAxis(impl.controller_, camera);

	float moveSpeed = 0.045f;

	if(move.LengthSquare() > 0.0f){
		// 加速
		if(impl.doping_){
			moveSpeed += 0.03f;
		} else if(impl.exhausted_ == 0 && impl.stamina_ >= 0.9f && impl.controller_.GetButton(PadButton::R)){
			moveSpeed += 0.02f;

			// プレビュー時はスタミナ消費なし
			if(typeid(scene) != typeid(Preview)){
				impl.stamina_ = Max(0.0f, impl.stamina_ - 1.0f);

				if(impl.stamina_ <= 0.0f){
					impl.exhausted_ = 210;
				}
			}
		}

		// 疲労時減速
		if(impl.exhausted_){
			moveSpeed -= 0.015f;
		}

		impl.moveVector_ += move * moveSpeed;

		// モデルの向きを変える
		Vector2D rotationVector = { move.ReplaceY(0).Normalized().x, move.ReplaceY(0).Normalized().z };
		main_.rotation_.y = -(rotationVector.Angle() + ideaMath::PIDIV2);
	} else{
		main_.SetState(PlayerState::Idol);
	}

	impl.model_.UpdateVmd(impl.motionWalk_, moveSpeed * 30.0f);
}

void Player::Impl::Back::Update()
{
	// 実装
	auto& impl = (*main_.pImpl_);

	// のけぞり切ったらIdolへ移行
	if(impl.backVector_.LengthSquare() <= 0.01f){
		main_.SetState(PlayerState::Idol);
	}

	impl.model_.UpdateVmd(impl.motionIdol_, 1.0f, 0.1f, true, false);
}

void Player::Impl::Back::UnInit()
{
	// 実装
	auto& impl = (*main_.pImpl_);

	impl.backVector_ = Vector3DZero();
}

void Player::Impl::Count::Init()
{
	// 実装
	auto& impl = (*main_.pImpl_);

	// カウントは約2.5秒
	count_ = 8 * 20;

	// カウントビルボード表示
	impl.oniCount_.SetColor(ideaColor::WHITE);
}

void Player::Impl::Count::Update()
{
	// 実装
	auto& impl = (*main_.pImpl_);

	--count_;

	impl.oniCount_.DrawFormatText("%d", (count_ + 20) / 50);

	// カウントが終わったらIdolへ移行
	if(count_ <= 0){
		main_.SetState(PlayerState::Idol);
	}

	impl.model_.UpdateVmd(impl.motionIdol_, 1.0f, 0.1f, true, false);
}

void Player::Impl::Count::UnInit()
{
	// 実装
	auto& impl = (*main_.pImpl_);

	// カウントビルボード非表示
	impl.oniCount_.SetColor(ideaColor::CLEAR);
}