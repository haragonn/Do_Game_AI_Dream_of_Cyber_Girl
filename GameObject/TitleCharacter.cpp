#include "TitleCharacter.h"
#include "TitleRenderer.h"

using namespace std;
using namespace idea;

class TitleCharacter::Impl{
public:
	// プレイヤーモデル
	PmxModel modelPlayer_;

	// vitaモデル
	PmxModel modelVita_;

	// 待機モーション
	VmdMotion motionIdol_;

	// 移動モーション
	VmdMotion motionWalk_;

	// 追いかけフラグ
	bool bChase_ = false;

	// 待機状態
	class Idol : public State<TitleCharacterState>{
	public:
		Idol() = delete;
		Idol(TitleCharacter& main) :
			State<TitleCharacterState>(TitleCharacterState::Idol),
			main_(main){}

		// 初期化
		void Init() override;

		// 更新
		void Update() override;

		// 終了処理
		void UnInit() override;

	private:
		// 親
		TitleCharacter& main_;

		// 状態遷移カウント
		int transitionCount_ = 0;
	};

	// 追いかける状態
	class Chase : public State<TitleCharacterState>{
	public:
		Chase() = delete;
		Chase(TitleCharacter& main) :
			State<TitleCharacterState>(TitleCharacterState::Chase),
			main_(main){}

		// 初期化
		void Init() override;

		// 更新
		void Update() override;

	private:
		// 親
		TitleCharacter& main_;

		// 状態遷移カウント
		int transitionCount_ = 0;
	};

	// 逃げる状態
	class Escape : public State<TitleCharacterState>{
	public:
		Escape() = delete;
		Escape(TitleCharacter& main) :
			State<TitleCharacterState>(TitleCharacterState::Escape),
			main_(main){}

		// 初期化
		void Init() override;

		// 更新
		void Update() override;

	private:
		// 親
		TitleCharacter& main_;

		// 状態遷移カウント
		int transitionCount_ = 0;
	};
};

TitleCharacter::TitleCharacter() :
	pImpl_(new TitleCharacter::Impl)
{
	Assert(pImpl_);
}

TitleCharacter::~TitleCharacter()
{
	SafeDelete(pImpl_);
}

void TitleCharacter::Init()
{
	// 実装
	auto& impl = (*pImpl_);

	// シーン
	auto& scene = (*pScene_);

	// レンダラー
	auto& renderer = (*scene.GetGameObject<TitleRenderer>(RENDERER_LAYER));

	impl.modelPlayer_.Init(5.0f, 0.0f, 0.0f);
	impl.modelPlayer_.SetScale(0.3f, 0.3f, 0.3f);
	impl.modelPlayer_.SetRotation(0.0f, DegreeToRadian(90.0f), 0.0f);
	impl.modelPlayer_.LoadPmxMeshFromStorage("asset/MODEL/Player/Player.pmx");
	renderer.AddObject(impl.modelPlayer_);

	impl.modelVita_.Init(0.0f, 0.0f, 0.0f);
	impl.modelVita_.SetScale(0.3f, 0.3f, 0.3f);
	impl.modelVita_.SetRotation(0.0f, DegreeToRadian(90.0f), 0.0f);
	impl.modelVita_.LoadPmxMeshFromStorage("asset/MODEL/Vita/Vita.pmx");
	renderer.AddObject(impl.modelVita_);

	impl.motionIdol_.LoadVmdMotionFromFile("asset/MOTION/_待機.vmd");
	impl.motionWalk_.LoadVmdMotionFromFile("asset/MOTION/歩く.vmd");

	AddState(make_shared<TitleCharacter::Impl::Idol>(*this));
	AddState(make_shared<TitleCharacter::Impl::Chase>(*this));
	AddState(make_shared<TitleCharacter::Impl::Escape>(*this));

	SetState(TitleCharacterState::Idol);

	impl.bChase_ = true;
}

void TitleCharacter::Update()
{
	StateMachine::Update();
}

void TitleCharacter::Impl::Idol::Init()
{
	transitionCount_ = STATE_TIME;
}

void TitleCharacter::Impl::Idol::Update()
{
	// 実装
	auto& impl = (*main_.pImpl_);

	--transitionCount_;

	if(!transitionCount_){
		if(impl.bChase_){
			main_.SetState(TitleCharacterState::Escape);
		} else{
			main_.SetState(TitleCharacterState::Chase);
		}
	}

	impl.modelPlayer_.UpdateVmd(impl.motionIdol_, 1.0f, 0.03f);
	impl.modelVita_.UpdateVmd(impl.motionIdol_, 1.0f, 0.03f);
}

void TitleCharacter::Impl::Idol::UnInit()
{
	// 実装
	auto& impl = (*main_.pImpl_);

	impl.bChase_ = !impl.bChase_;
}

void TitleCharacter::Impl::Chase::Init()
{
	// 実装
	auto& impl = (*main_.pImpl_);

	impl.modelPlayer_.SetRotation(0.0f, DegreeToRadian(-90.0f), 0.0f);
	impl.modelVita_.SetRotation(0.0f, DegreeToRadian(-90.0f), 0.0f);

	transitionCount_ = STATE_TIME;
}

void TitleCharacter::Impl::Chase::Update()
{
	// 実装
	auto& impl = (*main_.pImpl_);

	--transitionCount_;

	if(!transitionCount_){
		main_.SetState(TitleCharacterState::Idol);
	}

	impl.modelPlayer_.UpdateVmd(impl.motionWalk_, 1.0f, 0.075f);
	impl.modelVita_.UpdateVmd(impl.motionWalk_, 1.0f, 0.075f);
}

void TitleCharacter::Impl::Escape::Init()
{
	// 実装
	auto& impl = (*main_.pImpl_);

	impl.modelPlayer_.SetRotation(0.0f, DegreeToRadian(90.0f), 0.0f);
	impl.modelVita_.SetRotation(0.0f, DegreeToRadian(90.0f), 0.0f);

	transitionCount_ = STATE_TIME;
}

void TitleCharacter::Impl::Escape::Update()
{
	// 実装
	auto& impl = (*main_.pImpl_);

	--transitionCount_;

	if(!transitionCount_){
		main_.SetState(TitleCharacterState::Idol);
	}

	impl.modelPlayer_.UpdateVmd(impl.motionWalk_, 1.0f, 0.075f);
	impl.modelVita_.UpdateVmd(impl.motionWalk_, 1.0f, 0.075f);
}