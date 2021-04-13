#include "TouchEffect.h"
#include "Renderer.h"

using namespace idea;

class TouchEffect::Impl{
public:
	// �G�t�F�N�g�\���r���{�[�h
	Billboard billboardEffect_;

	// �G�t�F�N�g�̃e�N�X�`���[
	Texture texrueEffect_;

	// �^�b�`�����ۂ̌��ʉ�
	Sound seTouch_;

	// �A�j���[�V�����J�E���g
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
	// ����
	auto& impl = (*pImpl_);

	// �V�[��
	auto& scene = (*pScene_);

	// �����_���[
	auto& renderer = (*scene.GetGameObject<Renderer>(RENDERER_LAYER));

	impl.billboardEffect_.Init(0, -10000, 0);
	impl.billboardEffect_.SetScale(25, 25, 25);

	renderer.GetWrapCamera().AddObject(impl.billboardEffect_);

	impl.texrueEffect_.LoadImageFromStorage("asset/TEXTURE/explosion.png", 4, 4);

	impl.animeCount_ = 0;
}

void TouchEffect::Update()
{
	// ����
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
	// ����
	auto& impl = (*pImpl_);

	impl.billboardEffect_.SetColor(color);
}