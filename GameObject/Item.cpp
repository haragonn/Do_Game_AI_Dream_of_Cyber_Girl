#include "Item.h"
#include "Renderer.h"

using namespace idea;

class Item::Impl{
public:
	// �Z�����W
	Cell cell_;

	// �ړ��x�N�g��
	Vector3D moveVector_;

	// ���̓����蔻��
	Sphere sphereCollision_;

	// �\���p�r���{�[�h
	Billboard billboardItem_;

	// �A�C�e���̃e�N�X�`��
	Texture textureItem_;

	// ����
	int life_ = 0;

	// �f�o�b�O���
	Debug debugLog_;
};

Item::Item() :
	pImpl_(new Item::Impl)
{
	Assert(pImpl_);
}

Item::~Item()
{
	SafeDelete(pImpl_);
}

void Item::Init()
{
	// ����
	auto& impl = (*pImpl_);

	// �V�[��
	auto& scene = (*pScene_);

	// �����_���[
	auto& renderer = (*scene.GetGameObject<Renderer>(RENDERER_LAYER));

	position_ = Vector3D(0.0f, -10.0f, 0.0f);

	impl.textureItem_.LoadImageFromStorage("asset/TEXTURE/item.png");

	impl.sphereCollision_.Init(position_);
	impl.sphereCollision_.SetScale(4.0f, 4.0f, 4.0f);
	renderer.AddObject(impl.sphereCollision_);

	impl.billboardItem_.Init(position_);
	impl.billboardItem_.SetScale(3.0f, 3.0f, 3.0f);
	impl.billboardItem_.SetYBillboardFlag(true);
	impl.billboardItem_.SetColor(ideaColor::CLEAR);
	impl.billboardItem_.SetTexture(impl.textureItem_);
	renderer.AddObject(impl.billboardItem_);

	impl.life_ = 900;
}

void Item::Update()
{
	// ����
	auto& impl = (*pImpl_);

	// ���Ԍo�߂ŏ���
	if(impl.life_){
		--impl.life_;
	} else{
		SetDestroy();

		return;
	}

	impl.billboardItem_.SetColor(ideaColor::WHITE.ReplaceAlpha(impl.life_ / 120.0f));

	impl.moveVector_.y = impl.moveVector_.y - 0.15f;

	position_ += impl.moveVector_;

	position_.y = Max(2.0f, position_.y);

	impl.sphereCollision_.SetPosition(position_);
	impl.billboardItem_.SetPosition(position_);

	if(impl.debugLog_.GetDebugSwitch(0)){
		impl.sphereCollision_.SetColor(ideaColor::TRANSLUCENT);
	} else{
		impl.sphereCollision_.SetColor(ideaColor::CLEAR);
	}
}

void Item::HitObstacle(const Vector3D& axis)
{
	// ����
	auto& impl = (*pImpl_);

	//�d�͏�����
	impl.moveVector_.y = 0.0f;

	position_ += axis;

	impl.sphereCollision_.SetPosition(position_);
	impl.billboardItem_.SetPosition(position_);
}

void Item::SetCell(int x, int y)
{
	pImpl_->cell_.x = x;
	pImpl_->cell_.y = y;
}

Cell& Item::GetCell()
{
	return pImpl_->cell_;
}

SphereCollider Item::GetCollider()
{
	return pImpl_->sphereCollision_.GetCollider();
}
