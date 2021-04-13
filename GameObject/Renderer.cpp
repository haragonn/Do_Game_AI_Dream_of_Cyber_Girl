#include "Renderer.h"
#include "Player.h"

using namespace idea;

class Renderer::Impl{
public:
	// ライト
	Light light_;

	// カメラ
	WrapCamera wrapCamera_;

	// 影描画用カメラ
	ShadowCamera shadowCamera_;

	// 影描画用カメラ
	ShadowCamera nonShadowCamera_;

	// コントローラー情報
	Controller controller_;

	// 注視点
	Vector3D target_;

	// 開始直後か
	bool isIntro_ = false;

	// デバッグログ
	Debug debugLog_;
};

Renderer::Renderer() :
	pImpl_(new Renderer::Impl)
{
	Assert(pImpl_);
}

Renderer::~Renderer()
{
	SafeDelete(pImpl_);
}

void Renderer::Init()
{
	// 実装
	auto& impl = (*pImpl_);

	impl.light_.SetLightDirection(-3.0f, -3.0f, 0.0f);

	impl.wrapCamera_.Init(DegreeToRadian(60), S_W / S_H, 0.1f, 10000.0f, 20.0f);
	impl.wrapCamera_.SetFocus(0.0f, -20.0f, 0.0f);
	impl.wrapCamera_.SetRotation(0.0f, DegreeToRadian(15));
	impl.wrapCamera_.SetPitchThreshold(DegreeToRadian(1.0f), DegreeToRadian(90.0f));
	impl.wrapCamera_.SetViewPort(0.0f, 0.0f, S_W, S_H);
	impl.wrapCamera_.SetColliderRadius(2.5f);

	impl.shadowCamera_.Init(22, 5.0f, 100.0f);

	impl.nonShadowCamera_.Init(100, 50.0f, 1000.0f);

	impl.controller_.Init(1, true);
	impl.controller_.SetConfig(PadButton::BACK, KeyCode::BACKSPACE);

	impl.target_ = Vector3D(0.0f, 5.0f, -40.0f);

	impl.isIntro_ = true;
}

void Renderer::Update()
{
	// 実装
	auto& impl = (*pImpl_);

	// シーン
	auto& scene = (*pScene_);

	// プレイヤーの位置
	auto& playerPosition = scene.GetGameObject<Player>(PLAYER_LAYER)->GetPosition();

	impl.controller_.Update();

	// カメラスピード
	float cameraSpeed = 0.02f;

	Vector2D cm(-impl.controller_.GetRAxisX(), impl.controller_.GetRAxisY() * 0.8f);
	cm *= cameraSpeed;
	impl.wrapCamera_.MoveRotation(cm.x, cm.y);

	// プレイヤーのちょっと上を見る
	impl.target_ += (playerPosition.ReplaceY(playerPosition.y + 3.5f) - impl.wrapCamera_.GetFocus()) * 0.2f;

	impl.wrapCamera_.SetFocus(impl.target_);

	if(impl.isIntro_){
		impl.target_ = playerPosition.ReplaceY(playerPosition.y + 3.5f);
		impl.wrapCamera_.SetFocus(impl.target_);
		impl.isIntro_ = false;
	}
}

void Renderer::Draw()
{
	// 実装
	auto& impl = (*pImpl_);

	if(impl.debugLog_.GetDebugSwitch(2)){
		impl.nonShadowCamera_.DrawObject();
	} else{
		impl.shadowCamera_.DrawObject();
	}

	impl.wrapCamera_.DrawObject();
}

idea::WrapCamera& Renderer::GetWrapCamera()
{
	return pImpl_->wrapCamera_;
}

idea::ShadowCamera& Renderer::GetShadowCamera()
{
	return pImpl_->shadowCamera_;
}

void Renderer::AddObject(Object& object)
{
	// 実装
	auto& impl = (*pImpl_);

	impl.wrapCamera_.AddObject(object);
	impl.shadowCamera_.AddObject(object);
}
