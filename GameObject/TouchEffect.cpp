#include "TouchEffect.h"
#include "Renderer.h"

using namespace idea;

class TouchEffect::Impl{
public:
	// エフェクト表示ビルボード
	Billboard billboardEffect_;

	// エフェクトのテクスチャー
	Texture texrueEffect_;

	// タッチした際の効果音
	Sound seTouch_;

	// アニメーションカウント
	int animeCount_ = 0;
};

TouchEffect::TouchEffect() :
	pImpl_(new TouchEffect::Impl)
{
	Assert(pImpl_);
}

TouchEffect::~TouchEffect()
{
	SafeDelete(pImpl_);
}

void TouchEffect::Init()
{
	// 実装
	auto& impl = (*pImpl_);

	// シーン
	auto& scene = (*pScene_);

	// レンダラー
	auto& renderer = (*scene.GetGameObject<Renderer>(RENDERER_LAYER));

	impl.billboardEffect_.Init(0, -10000, 0);
	impl.billboardEffect_.SetScale(25, 25, 25);

	renderer.GetWrapCamera().AddObject(impl.billboardEffect_);

	impl.texrueEffect_.LoadImageFromStorage("asset/TEXTURE/explosion.png", 4, 4);

	impl.animeCount_ = 0;
}

void TouchEffect::Update()
{
	// 実装
	auto& impl = (*pImpl_);

	impl.billboardEffect_.SetPosition(position_);

	if(impl.animeCount_ >= 32){
		SetDestroy();
		return;
	}

	if(impl.animeCount_ >= 16){
		impl.billboardEffect_.SetDividedTexture(impl.texrueEffect_, 3, 3);
	} else{
		impl.billboardEffect_.SetDividedTexture(impl.texrueEffect_, impl.animeCount_ % 4, impl.animeCount_ / 4);
	}

	++impl.animeCount_;
}

void TouchEffect::SetColor(Color color)
{
	// 実装
	auto& impl = (*pImpl_);

	impl.billboardEffect_.SetColor(color);
}