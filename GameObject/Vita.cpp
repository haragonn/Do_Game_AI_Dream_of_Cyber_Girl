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
	// 前フレームの座標
	Vector3D prePosition_;

	// 移動ベクトル
	Vector3D moveVector_;

	// のけぞりベクトル
	Vector3D backVector_;

	// 役割
	VitaRole role_ = VitaRole::Main;

	// 追いかける速度のレート
	float chaseSpeedRate_ = 0.0f;

	// セル座標
	Cell cell_;

	// 最短経路情報
	WayPointAStar wayPointAStar_;

	// モデル
	PmxModel model_;

	// 待機モーション
	VmdMotion motionIdol_;

	// 移動モーション
	VmdMotion motionWalk_;

	// コライダー
	CapsuleMesh capsuleCollision_;

	// 登場時のエフェクト表示用ビルボード
	Billboard billboardAppearanceEffect_;

	// 登場時のエフェクトのテクスチャー
	Texture textureAppearanceEffect_;

	// 待機カウント時の数字表示用フォント
	BillboardFont oniCount_;

	// デバッグ情報
	Debug debugLog_;

	// 待機状態
	class Idol : public State<VitaState>{
	public:
		Idol() = delete;
		Idol(Vita& main) :
			State<VitaState>(VitaState::Idol),
			main_(main)
		{
		}

		// 初期化
		void Init() override;

		// 更新
		void Update() override;

		// 状態遷移
		bool TransitionState();

	private:
		// 親
		Vita& main_;
	};

	// 近づく状態
	class StopBy : public State<VitaState>{
	public:
		StopBy() = delete;
		StopBy(Vita& main) :
			State<VitaState>(VitaState::StopBy),
			main_(main)
		{
		}

		// 更新
		void Update() override;

		// 状態遷移
		bool TransitionState();

	private:
		// 親
		Vita& main_;
	};

	// 追いかける状態
	class Chase : public State<VitaState>{
	public:
		Chase() = delete;
		Chase(Vita& main) :
			State<VitaState>(VitaState::Chase),
			main_(main)
		{
		}

		// 初期化
		void Init() override;

		// 更新
		void Update() override;

		// 終了処理
		void UnInit() override;

		// 状態遷移
		bool TransitionState();

		// 移動
		void Move();

	private:
		// 親
		Vita& main_;

		// 間隔をあけるフラグ
		bool bSocial_ = false;

		// 間隔を開けておく時間
		int socialCount_ = 0;
	};

	// 逃げる状態
	class Escape : public State<VitaState>{
	public:
		Escape() = delete;
		Escape(Vita& main) :
			State<VitaState>(VitaState::Escape),
			main_(main)
		{
		}

		// 初期化
		void Init() override;

		// 更新
		void Update() override;

		// 終了処理
		void UnInit() override;

		// 状態遷移
		bool TransitionState();

		// 移動
		void Move();

		// 逃走先方向の取得
		Vector3D GetEscapeDirection();

	private:
		// 親
		Vita& main_;

		// 目標座標
		Vector3D targetPosition_;

		// 目標ポイント
		Vector3D targetPoint_;
	};

	// 離脱状態
	class Withdrawal : public State<VitaState>{
	public:
		Withdrawal() = delete;
		Withdrawal(Vita& main) :
			State<VitaState>(VitaState::Withdrawal),
			main_(main)
		{
		}

		// 初期化
		void Init() override;

		// 更新
		void Update() override;

		// 終了処理
		void UnInit() override;

		// 状態遷移
		bool TransitionState();

		// 移動
		void Move();

		// 目標方向の取得
		Vector3D GetTargetDirection();

	private:
		// 親
		Vita& main_;

		// 目標座標
		Vector3D targetPosition_;

		// 目標ポイント
		Vector3D targetPoint_;
	};

	// カウント状態
	class Count : public State<VitaState>{
	public:
		Count() = delete;
		Count(Vita& main) :
			State<VitaState>(VitaState::Count),
			main_(main)
		{
		}

		// 初期化
		void Init() override;

		// 更新
		void Update() override;

		// 終了処理
		void UnInit() override;

		// 状態遷移
		bool TransitionState();

	private:
		// 親
		Vita& main_;

		// カウント時間
		int count_ = 0;
	};

	// 登場状態
	class Appearance : public State<VitaState>{
	public:
		Appearance() = delete;
		Appearance(Vita& main) :
			State<VitaState>(VitaState::Appearance),
			main_(main)
		{
		}

		// 初期化
		void Init() override;

		// 更新
		void Update() override;

		// 終了処理
		void UnInit() override;

		// 状態遷移
		bool TransitionState();

	private:
		// 親
		Vita& main_;

		// カウント時間
		int count_ = 0;
	};

	// 消滅状態
	class Disappearance : public State<VitaState>{
	public:
		Disappearance() = delete;
		Disappearance(Vita& main) :
			State<VitaState>(VitaState::Disappearance),
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
		Vita& main_;

		// カウント時間
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
	// 実装
	auto& impl = (*pImpl_);

	// シーン
	auto& scene = (*pScene_);

	// エディットモードのvitaはモデルの実装だけ
	if(typeid(scene) == typeid(EditMode)){
		// レンダラー
		auto& renderer = (*scene.GetGameObject<EditRenderer>(EDIT_RENDERER_LAYER));

		impl.model_.Init(position_);
		impl.model_.SetScale(0.7f, 0.7f, 0.7f);
		impl.model_.SetRotation(0.0f, DegreeToRadian(0), 0.0f);
		impl.model_.LoadPmxMeshFromStorage("asset/MODEL/Vita/Vita.pmx");
		renderer.AddObject(impl.model_);

		impl.motionIdol_.LoadVmdMotionFromStorage("asset/MOTION/_待機.vmd");

		return;
	}

	// レンダラー
	auto& renderer = (*scene.GetGameObject<Renderer>(RENDERER_LAYER));

	// マップ情報
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

	impl.motionIdol_.LoadVmdMotionFromStorage("asset/MOTION/_待機.vmd");
	impl.motionWalk_.LoadVmdMotionFromStorage("asset/MOTION/歩く.vmd");

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

	// プレビュー時にカウントは不要
	if(typeid(scene) == typeid(Preview)){
		SetState(VitaState::Chase);
	} else{
		SetState(VitaState::Count);
	}
}

void Vita::Update()
{
	// 実装
	auto& impl = (*pImpl_);

	// シーン
	auto& scene = (*pScene_);

	impl.model_.SetColor(ideaColor::WHITE);

	// エディットモードなら止まっているだけ
	if(typeid(scene) == typeid(EditMode)){
		impl.model_.SetPosition(position_);
		impl.model_.UpdateVmd(impl.motionIdol_, 1.0f, 0.1f, true, false);

		return;
	}

	// メタAI情報
	auto& metaAI = (*scene.GetGameObject<MetaAI>(META_AI_LAYER));

	// 坂の上り下りで移動速度を変化
	if((position_ - impl.prePosition_).y > 0.005f){
		position_ -= impl.moveVector_ * metaAI.GetVitaClimbRate();
	} else if((position_ - impl.prePosition_).y < -0.2f){
		position_ += impl.moveVector_ * 0.5f;
	}

	// 前フレームの座標を保持
	impl.prePosition_ = position_;

	// ステートマシンの更新
	StateMachine::Update();

	// 移動
	impl.moveVector_.y = Min(0.0f, impl.moveVector_.y);

	impl.moveVector_.y = impl.moveVector_.y - 0.2f;

	impl.moveVector_ *= 0.83f;

	impl.backVector_ *= 0.94f;

	position_ += impl.moveVector_ + impl.backVector_.ReplaceY(0);

	// 床より低くならないよう
	if(position_.y < 0.0f){
		position_.y = 0.0f;
		impl.moveVector_.y = 0.0f;
	}

	impl.model_.SetPosition(position_);
	impl.model_.SetRotation(rotation_);

	impl.capsuleCollision_.SetPosition(position_.ReplaceY(position_.y + 2.5f));

	impl.oniCount_.SetPosition(position_.ReplaceY(position_.y + 6.0f));

	// 役職の表示
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
	// 実装
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
	// 実装
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
	// 実装
	auto& impl = (*pImpl_);

	// シーン
	auto& scene = (*pScene_);

	// プレイヤー
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	return (position_ - player.GetPosition()).Normalized() * Vector3D(30.0f, 0.0f, 50.0f);
}

void Vita::SetChaseSpeedRate(float rate)
{
	pImpl_->chaseSpeedRate_ = rate;
}

void Vita::Impl::Idol::Init()
{
	// 実装
	auto& impl = (*main_.pImpl_);

	// 最短経路情報
	auto& astar = impl.wayPointAStar_;

	astar.Reset();

	impl.moveVector_ = Vector3DZero();

	impl.model_.UpdateVmd(impl.motionIdol_, 2.5f, 0.1f, true, false);
}

void Vita::Impl::Idol::Update()
{
	// 実装
	auto& impl = (*main_.pImpl_);

	// シーン
	auto& scene = (*main_.pScene_);

	// プレイヤー
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// 状態遷移
	if(TransitionState()){
		return;
	}

	// 方向転換
	Vector3D direction = (player.GetPosition() - main_.position_).ReplaceY(0).Normalized();

	if(direction.LengthSquare() > 0.0f){
		// 方向ベクトル
		Vector2D rotationVector = { direction.ReplaceY(0).Normalized().x, direction.ReplaceY(0).Normalized().z };
		main_.rotation_.y = -(rotationVector.Angle() + ideaMath::PIDIV2);
	}

	impl.model_.UpdateVmd(impl.motionIdol_, 2.5f, 0.1f, true, false);
}

bool Vita::Impl::Idol::TransitionState()
{
	// 実装
	auto& impl = (*main_.pImpl_);

	// シーン
	auto& scene = (*main_.pScene_);

	// プレイヤー
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// メタAI情報
	auto& metaAI = (*scene.GetGameObject<MetaAI>(META_AI_LAYER));

	if(main_.GetRole() == VitaRole::Main){
		// 本体が待機しているときは逃げるとき
		if((player.GetPosition() - main_.position_).ReplaceY(0).LengthSquare() < metaAI.GetVitaIdolRange() * metaAI.GetVitaIdolRange()){
			main_.SetState(VitaState::Escape);
			return true;
		}
	} else{
		// 本体以外は捕まえに行く
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
	// 実装
	auto& impl = (*main_.pImpl_);

	// シーン
	auto& scene = (*main_.pScene_);

	// プレイヤー
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// メタAI情報
	auto& metaAI = (*scene.GetGameObject<MetaAI>(META_AI_LAYER));

	if(TransitionState()){
		return;
	}

	// 追従方向
	Vector3D moveDirection = (player.GetPosition() - main_.position_).ReplaceY(0).Normalized();

	// 移動速度
	float moveSpeed = 0.0f;

	if(moveDirection.LengthSquare() > 0.0f){
		// 移動
		auto& metaAI = (*scene.GetGameObject<MetaAI>(META_AI_LAYER));

		moveSpeed = metaAI.GetVitaStopBySpeed();

		impl.moveVector_ += moveDirection * moveSpeed;

		// 方向転換
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
	// 実装
	auto& impl = (*main_.pImpl_);

	// シーン
	auto& scene = (*main_.pScene_);

	// プレイヤー
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// プレイヤーと離れてしまったら
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
	// 実装
	auto& impl = (*main_.pImpl_);

	// シーン
	auto& scene = (*main_.pScene_);

	// プレイヤー
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// vita本体
	auto& vita = (*scene.GetGameObject<Vita>(VITA_LAYER));

	// 最短経路情報
	auto& astar = impl.wayPointAStar_;

	if(impl.role_ == VitaRole::Wrap
		|| impl.role_ == VitaRole::Ambush){
		// 本体と距離を置く
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
	// 実装
	auto& impl = (*main_.pImpl_);

	// 最短経路情報
	auto& astar = impl.wayPointAStar_;

	astar.Reset();
}

bool Vita::Impl::Chase::TransitionState()
{
	// 実装
	auto& impl = (*main_.pImpl_);

	// シーン
	auto& scene = (*main_.pScene_);

	// プレイヤー
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// vita本体
	auto& vita = (*scene.GetGameObject<Vita>(VITA_LAYER));

	// メタAI情報
	auto& metaAI = (*scene.GetGameObject<MetaAI>(META_AI_LAYER));

	// 最短経路情報
	auto& astar = impl.wayPointAStar_;

	// カメラ
	auto& camera = scene.GetGameObject<Renderer>(RENDERER_LAYER)->GetWrapCamera();

	switch(impl.role_){
		case VitaRole::Main:
		case VitaRole::Chase:
			// プレイヤーと距離が近ければ接近
			if((player.GetPosition() - main_.position_).ReplaceY(0).LengthSquare() < 8.0f * 8.0f
				&& (player.GetPosition().y - main_.position_.y) < 4.5f
				&& astar.IsGoal()){
				main_.SetState(VitaState::StopBy);
				return true;
			}
			break;

		case VitaRole::Wrap:
		case VitaRole::Ambush:
			// プレイヤーと距離が遠ければ離脱
			if((player.GetPosition() - main_.position_).ReplaceY(0).LengthSquare() > 25.0f * 25.0f
				&& socialCount_ > 5 * 60){
				main_.SetState(VitaState::Withdrawal);
				return true;
			}

			// カメラに写っていなければ離脱
			if(!camera.IsVisible(impl.capsuleCollision_)){
				main_.SetState(VitaState::Withdrawal);
				return true;
			}

			// プレイヤーと距離が近ければ接近
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
	// 実装
	auto& impl = (*main_.pImpl_);

	// シーン
	auto& scene = (*main_.pScene_);

	// プレイヤー
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// vita本体
	auto& vita = (*scene.GetGameObject<Vita>(VITA_LAYER));

	// メタAI情報
	auto& metaAI = (*scene.GetGameObject<MetaAI>(META_AI_LAYER));

	// 最短経路情報
	auto& astar = impl.wayPointAStar_;

	// 目標座標
	Vector3D targetPosition = astar.GetNextPosition(main_.position_, player.GetPosition().ReplaceY(player.GetPosition().y + 3.5f), 0.2f).ReplaceY(0);

	// 移動方向
	Vector3D moveDirection = (targetPosition - main_.position_).ReplaceY(0).Normalized();

	// 本体以外は
	if(impl.role_ != VitaRole::Main){
		// vita本体に近ければ離れるよう設定
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

	// 移動速度
	float moveSpeed = 0.0f;

	if(moveDirection.LengthSquare() > 0.0f){
		// スピード調整
		moveSpeed = metaAI.GetVitaChaseSpeed();
		if(impl.role_ != VitaRole::Main){
			moveSpeed = metaAI.GetVitaChaseSpeed() * impl.chaseSpeedRate_;
		}

		impl.moveVector_ += moveDirection * moveSpeed;

		// 方向転換
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
	// 逃走先の方向
	Vector3D escapeDirection = GetEscapeDirection();

	targetPoint_ = (escapeDirection).Normalized() * Vector3D(30.0f, 0.0f, 50.0f);

	// 現在地と近ければ条件を変える
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
	// 実装
	auto& impl = (*main_.pImpl_);

	// 最短経路情報
	auto& astar = impl.wayPointAStar_;

	astar.Reset();
}

bool Vita::Impl::Escape::TransitionState()
{
	// 実装
	auto& impl = (*main_.pImpl_);

	// 本体以外が逃げているのはおかしいので強制消滅
	if(impl.role_ != VitaRole::Main){
		main_.SetState(VitaState::Disappearance);
		return true;
	}

	return false;
}

void Vita::Impl::Escape::Move()
{
	// 実装
	auto& impl = (*main_.pImpl_);

	// シーン
	auto& scene = (*main_.pScene_);

	// プレイヤー
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// メタAI情報
	auto& metaAI = (*scene.GetGameObject<MetaAI>(META_AI_LAYER));

	// 最短経路情報
	auto& astar = impl.wayPointAStar_;

	if(astar.IsGoal()){
		// 待機範囲より広ければ逃げる
		if((main_.position_ - player.GetPosition()).ReplaceY(0).LengthSquare() < metaAI.GetVitaIdolRange() * metaAI.GetVitaIdolRange()){
			// 逃走先の方向
			Vector3D escapeDirection = GetEscapeDirection();

			targetPoint_ = (escapeDirection).Normalized() * Vector3D(30.0f, 0.0f, 50.0f);

			// 現在地と近ければ条件を変える
			if((main_.position_ - targetPoint_).LengthSquare() < 15.0f * 15.0f){
				targetPoint_ = (escapeDirection).Normalized() * Vector3D(50.0f, 0.0f, 30.0f);
			}
		} else{
			// 待機状態に遷移
			main_.SetState(VitaState::Idol);

			return;
		}
	}

	targetPosition_ = astar.GetNextPosition(main_.position_, targetPoint_, 0.2f).ReplaceY(0);

	// 移動方向
	Vector3D moveDirection = (targetPosition_ - main_.position_).ReplaceY(0).Normalized();

	// 移動できなかったら
	if(moveDirection == Vector3DZero()){
		// 目標拡大レート
		float rate = 1.0f;

		// 符号
		int code = 1;

		// 逃走先の方向
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

	// 移動速度
	float moveSpeed = 0.0f;

	if(moveDirection.LengthSquare() > 0.0f){
		moveSpeed = metaAI.GetVitaEscapeSpeed();

		impl.moveVector_ += moveDirection * moveSpeed;

		// 方向転換
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
	// シーン
	auto& scene = (*main_.pScene_);

	// プレイヤー
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// 逃走先の方向
	Vector3D escapeDirection = (main_.position_ - player.GetPosition()).ReplaceY(0).Normalized();

	// 差分が少なければ移動できるように補正
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
	// 目標の方向
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
	// 実装
	auto& impl = (*main_.pImpl_);

	// 最短経路情報
	auto& astar = impl.wayPointAStar_;

	astar.Reset();
}

bool Vita::Impl::Withdrawal::TransitionState()
{
	// 実装
	auto& impl = (*main_.pImpl_);

	// シーン
	auto& scene = (*main_.pScene_);

	// プレイヤー
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// 最短経路情報
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
	// 実装
	auto& impl = (*main_.pImpl_);

	// シーン
	auto& scene = (*main_.pScene_);

	// プレイヤー
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// vitaリスト
	auto vitas = scene.GetGameObjects<Vita>(VITA_LAYER);

	// メタAI情報
	auto& metaAI = (*scene.GetGameObject<MetaAI>(META_AI_LAYER));

	// 最短経路情報
	auto& astar = impl.wayPointAStar_;

	// ほかのvitaと競合したセルにいるか
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
		// 目標の方向
		Vector3D targetDirection = GetTargetDirection();

		targetPoint_ = (targetDirection).Normalized() * Vector3D(30.0f, 0.0f, 50.0f);

		if((main_.position_ - targetPoint_).LengthSquare() < 15.0f * 15.0f){
			targetPoint_ = (targetDirection).Normalized() * Vector3D(50.0f, 0.0f, 30.0f);
		}
	}

	targetPosition_ = astar.GetNextPosition(main_.position_, targetPoint_, 0.2f).ReplaceY(0);

	// 移動方向
	Vector3D moveDirection = (targetPosition_ - main_.position_).ReplaceY(0).Normalized();

	// 移動できなかったら
	if(moveDirection == Vector3DZero()){
		// 目標拡大レート
		float rate = 1.0f;

		// 符号
		int code = 1;

		// 逃走先の方向
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

	// 移動速度
	float moveSpeed = 0.0f;

	if(moveDirection.LengthSquare() > 0.0f){
		moveSpeed = metaAI.GetVitaEscapeSpeed();

		impl.moveVector_ += moveDirection * moveSpeed;

		// 方向転換
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
	// シーン
	auto& scene = (*main_.pScene_);

	// プレイヤー
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// vita本体
	auto& vita = (*scene.GetGameObject<Vita>(VITA_LAYER));

	// 目標の方向
	Vector3D targetDirection = (player.GetPosition() - vita.GetPosition()).ReplaceY(0).Normalized();

	// 差分が少なければ移動できるように補正
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
	// 実装
	auto& impl = (*main_.pImpl_);

	count_ = 8 * 20;

	impl.oniCount_.SetColor(ideaColor::CLEAR);
}

void Vita::Impl::Count::Update()
{
	// 実装
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
	// 実装
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
	// 実装
	auto& impl = (*main_.pImpl_);

	// シーン
	auto& scene = (*main_.pScene_);

	// レンダラー
	auto& renderer = (*scene.GetGameObject<Renderer>(RENDERER_LAYER));

	count_ = 0;

	impl.billboardAppearanceEffect_.SetScale(5.0f, 2000.0f, 0.0f);
	impl.billboardAppearanceEffect_.SetColor(ideaColor::CLEAR);

	impl.moveVector_ = Vector3DZero();

	impl.model_.UpdateVmd(impl.motionIdol_, 0.0f, 1.0f, true, false);
}

void Vita::Impl::Appearance::Update()
{
	// 実装
	auto& impl = (*main_.pImpl_);

	impl.billboardAppearanceEffect_.SetPosition(main_.position_.ReplaceY(main_.position_.y + 1000.0f));

	if(count_ < 60){
		impl.model_.SetColor(ideaColor::CLEAR);
	} else{
		impl.model_.SetColor(ideaColor::WHITE.ReplaceAlpha((count_) / 120.0f));
	}

	if(count_ > 150){
		// エフェクトの幅
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
	// 実装
	auto& impl = (*main_.pImpl_);

	impl.billboardAppearanceEffect_.SetColor(ideaColor::CLEAR);
}

bool Vita::Impl::Appearance::TransitionState()
{
	// 実装
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
	// 実装
	auto& impl = (*main_.pImpl_);

	count_ = 60;

	impl.billboardAppearanceEffect_.SetScale(0.0f, 2000.0f, 0.0f);
	impl.billboardAppearanceEffect_.SetColor(ideaColor::SOFT_BLUE.ReplaceAlpha(0.8f));

	impl.moveVector_ = Vector3DZero();

	impl.model_.UpdateVmd(impl.motionIdol_, 0.0f, 1.0f, true, false);
}

void Vita::Impl::Disappearance::Update()
{
	// 実装
	auto& impl = (*main_.pImpl_);

	impl.model_.SetColor(ideaColor::WHITE.ReplaceAlpha(count_ / 60.0f));

	impl.billboardAppearanceEffect_.SetPosition(main_.position_.ReplaceY(main_.position_.y + 1000.0f));

	// エフェクトの幅
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
	// 実装
	auto& impl = (*main_.pImpl_);

	impl.billboardAppearanceEffect_.SetColor(ideaColor::CLEAR);
}