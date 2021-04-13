#include "EditRenderer.h"

using namespace idea;

namespace{
	// �X�e�B�b�N���͂ƃL�[���͂̓���
	Vector2D GetMoveAxis(Controller controller)
	{
		Vector2D inputAxis = controller.GetLAxis();

		inputAxis.y = -inputAxis.y;

		inputAxis += controller.GetArrow();

		return inputAxis;
	}
}

class EditRenderer::impl{
public:
	// �J����
	WrapCamera wrapCamera_;

	// ���C�e�B���O���
	Light light_;

	// �R���g���[���[
	Controller controller_;

	// �}�E�X
	Mouse mouse_;

	// ��ʑ̂ƃJ�����̋���
	float distance_ = 0.0f;
};

EditRenderer::EditRenderer() :
	pImpl_(new EditRenderer::impl)
{
	Assert(pImpl_);
}

EditRenderer::~EditRenderer()
{
	SafeDelete(pImpl_);
}

void EditRenderer::Init()
{
	// ����
	auto& impl = (*pImpl_);

	impl.controller_.Init(1, true);

	impl.light_.SetLightDirection(1.2f, -3.0f, -0.2f);

	impl.distance_ = 160.0f;

	impl.wrapCamera_.Init(DegreeToRadian(40.0f), S_W / S_H, 0.1f, 1000.0f, impl.distance_);
	impl.wrapCamera_.SetFocus(0.0f, 0.0f, 0.0f);
	impl.wrapCamera_.SetRotation(0, DegreeToRadian(90.0f));
	impl.wrapCamera_.SetPitchThreshold(DegreeToRadian(-10.0f), DegreeToRadian(90.0f));
}

void EditRenderer::Update()
{
	// ����
	auto& impl = (*pImpl_);

	impl.controller_.Update();

	//impl.distance_ -= impl.mouse_.GetWheel() * 0.02f;

	//impl.distance_ = Clamp(impl.distance_, 10.0f, 160.0f);

	//impl.wrapCamera_.SetDistance(impl.distance_);

	Vector2D inputAxis = GetMoveAxis(impl.controller_);

	// �J�����X�s�[�h
	float cameraSpeed = 0.5f;

	// �J�����̈ړ�
	Vector2D move(inputAxis.x, inputAxis.y * 0.8f);
	move *= cameraSpeed;
	impl.wrapCamera_.MoveFocus(move.x, 0.0f, move.y);
}

void EditRenderer::Draw()
{
	pImpl_->wrapCamera_.DrawObject();
}

WrapCamera& EditRenderer::GetWrapCamera()
{
	return pImpl_->wrapCamera_;
}

void EditRenderer::AddObject(Object& object)
{
	pImpl_->wrapCamera_.AddObject(object);
}
