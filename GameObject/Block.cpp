#include "Block.h"
#include "Renderer.h"
#include "EditRenderer.h"

using namespace idea;

class Block::Impl{
public:
	// 箱ポリゴン
	Cube cube_;

	// テクスチャー
	Texture textureCube_;

	// ブロックの種類
	BlockStyle blockStyle_ = BlockStyle::Box;

	// セルの座標
	Cell cell_;
};

Block::Block() :
	pImpl_(new Block::Impl)
{
	Assert(pImpl_);
}

Block::~Block()
{
	SafeDelete(pImpl_);
}

void Block::Init()
{
	// 実装
	auto& impl = (*pImpl_);

	impl.textureCube_.LoadImageFromStorage("asset/TEXTURE/grid05.bmp");

	impl.cube_.Init(position_);

	impl.cube_.SetTexture(impl.textureCube_);

	impl.blockStyle_ = BlockStyle::Box;
}

void Block::Update()
{
	// 実装
	auto& impl = (*pImpl_);

	// シーン
	auto& scene = (*pScene_);

	// カメラのポインタ
	Camera* pCamera = nullptr;

	// レンダラーのポインタ
	auto pRenderer = scene.GetGameObject<Renderer>(RENDERER_LAYER);

	if(pRenderer){
		pCamera = &(pRenderer->GetWrapCamera());
	} else{
		// Rendererが存在しなければEditRendererを利用
		auto pEditRenderer = scene.GetGameObject<EditRenderer>(EDIT_RENDERER_LAYER);

		pCamera = &(pEditRenderer->GetWrapCamera());
	}

	impl.cube_.SetPosition(position_);
	impl.cube_.SetRotation(rotation_);
	impl.cube_.SetScale(scale_);

	// 色の設定
	impl.cube_.SetColor(ideaColor::WHITE);

	if(impl.blockStyle_ == BlockStyle::Wall1
		|| impl.blockStyle_ == BlockStyle::Wall2){
		impl.cube_.SetColor(ideaColor::SOFT_BLUE * 0.8f);
	}

	if(pCamera->GetCollider().CheckHit(impl.cube_.GetCollider())){
		impl.cube_.SetColor(ideaColor::TRANSLUCENT);
		if(impl.blockStyle_ == BlockStyle::Wall1
			|| impl.blockStyle_ == BlockStyle::Wall2){
			impl.cube_.SetColor(ideaColor::CLEAR);
		}
	}
}

void Block::SetStyle(BlockStyle style)
{
	// 実装
	auto& impl = (*pImpl_);

	// シーン
	auto& scene = (*pScene_);

	impl.blockStyle_ = style;

	// スタイルごとに変形
	switch(impl.blockStyle_){
		case BlockStyle::Box:
			scale_ = Vector3D(10.0f, 10.0f, 10.0f);
			break;

		case BlockStyle::UpperRight:
			position_.x += 0.8f;
			position_.y += 0.1f;
			rotation_ = Vector3D(0.0f, 0.0f, DegreeToRadian(26.5f));
			scale_ = Vector3D(12.0f, 5.0f, 9.95f);
			break;

		case BlockStyle::UpperLeft:
			position_.x -= 0.8f;
			position_.y += 0.1f;
			rotation_ = Vector3D(0.0f, DegreeToRadian(180.0f), DegreeToRadian(26.5f));
			scale_ = Vector3D(12.0f, 5.0f, 9.95f);
			break;

		case BlockStyle::UpperFront:
			position_.z -= 0.8f;
			position_.y += 0.1f;
			rotation_ = Vector3D(0.0f, DegreeToRadian(90), DegreeToRadian(26.5f));
			scale_ = Vector3D(12.0f, 5.0f, 9.95f);
			break;

		case BlockStyle::UpperBack:
			position_.z += 0.8f;
			position_.y += 0.1f;
			rotation_ = Vector3D(0.0f, DegreeToRadian(-90), DegreeToRadian(26.5f));
			scale_ = Vector3D(12.0f, 5.0f, 9.95f);
			break;

		case BlockStyle::Wall1:
			rotation_ = Vector3D(0.0f, 0.0f, 0.0f);
			impl.cube_.SetDelimitedTexture(impl.textureCube_, 0.0f, 0.0f, 6.5f, 1.0f);
			break;

		case BlockStyle::Wall2:
			rotation_ = Vector3D(0.0f, DegreeToRadian(-90), 0.0f);
			impl.cube_.SetDelimitedTexture(impl.textureCube_, 0.0f, 0.0f, 6.5f, 1.0f);
			break;
	}

	impl.cube_.SetPosition(position_);
	impl.cube_.SetRotation(rotation_);
	impl.cube_.SetScale(scale_);

	// レンダラーのポインタ
	auto pRenderer = scene.GetGameObject<Renderer>(RENDERER_LAYER);

	if(pRenderer){
		switch(impl.blockStyle_){
			// ブロックには影をつける
			case BlockStyle::Box:
			case BlockStyle::UpperRight:
			case BlockStyle::UpperLeft:
			case BlockStyle::UpperFront:
			case BlockStyle::UpperBack:
				pRenderer->AddObject(impl.cube_);
				break;

			// 壁には影をつけない
			case BlockStyle::Wall1:
			case BlockStyle::Wall2:
				pRenderer->GetWrapCamera().AddObject(impl.cube_);
				break;
		}

		impl.cube_.SetShadow(pRenderer->GetShadowCamera());
	} else{
		// Rendererが存在しなければEditRendererを利用
		auto pEditRenderer = scene.GetGameObject<EditRenderer>(EDIT_RENDERER_LAYER);

		pEditRenderer->AddObject(impl.cube_);
	}
}

BlockStyle Block::GetStyle()
{
	return pImpl_->blockStyle_;
}

Cube& Block::GetCube()
{
	return pImpl_->cube_;
}

BoxCollider Block::GetCollider()
{
	return pImpl_->cube_.GetCollider();
}

void Block::SetColor(Color color)
{
	pImpl_->cube_.SetColor(color);
}

void Block::SetCell(int x, int y)
{
	pImpl_->cell_.x = x;
	pImpl_->cell_.y = y;
}

Cell Block::GetCell()
{
	return pImpl_->cell_;
}
