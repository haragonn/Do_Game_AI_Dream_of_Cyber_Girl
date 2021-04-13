#include "TitleRenderer.h"

using namespace idea;

class TitleRenderer::Impl{
public:
	// ƒ‰ƒCƒg
	Light light_;

	// ƒJƒƒ‰
	WrapCamera wrapCamera_;
};

TitleRenderer::TitleRenderer() :
	pImpl_(new TitleRenderer::Impl)
{
	Assert(pImpl_);
}

TitleRenderer::~TitleRenderer()
{
	SafeDelete(pImpl_);
}

void TitleRenderer::Init()
{
	// ŽÀ‘•
	auto& impl = (*pImpl_);

	impl.light_.SetLightDirection(1.2f, -3.0f, -0.2f);

	impl.wrapCamera_.Init(DegreeToRadian(60), S_W / S_H, 0.1f, 100.0f, 2.75f);
	impl.wrapCamera_.SetFocus(-1.5f, 5.5f, 0.0f);
	impl.wrapCamera_.SetRotation(DegreeToRadian(225), DegreeToRadian(5));
	impl.wrapCamera_.SetViewPort(0.0f, 0.0f, S_W, S_H);
	impl.wrapCamera_.SetColliderRadius(2.5f);
}

void TitleRenderer::Draw()
{
	pImpl_->wrapCamera_.DrawObject();
}

WrapCamera& TitleRenderer::GetWrapCamera()
{
	return pImpl_->wrapCamera_;
}

void TitleRenderer::AddObject(Object& object)
{
	pImpl_->wrapCamera_.AddObject(object);
}
