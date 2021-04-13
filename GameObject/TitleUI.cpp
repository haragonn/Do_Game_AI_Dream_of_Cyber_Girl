#include "TitleUI.h"
#include "TitleCharacter.h"

using namespace idea;

class TitleUI::Impl{
public:
	// タイトルロゴのスプライト
	Sprite spriteTitle_;

	// タイトルロゴのテクスチャー
	Texture textureTitile_;

	// キー入力を促すフォントの背景
	Sprite spritePressKey_;

	// 吹き出しのスプライト
	Sprite spriteBalloon_;

	// 吹き出しのテクスチャー
	Texture textureBalloon_;

	// 吹き出しの動きのカウント
	int popUpBalloonCount_ = 0;

	// 状態表示用フォント
	Font fontState_;

	// キー入力を促すフォント
	Font fontPressKey_;
};

TitleUI::TitleUI() :
	pImpl_(new TitleUI::Impl)
{
	Assert(pImpl_);
}

TitleUI::~TitleUI()
{
	SafeDelete(pImpl_);
}

void TitleUI::Init()
{
	// 実装
	auto& impl = (*pImpl_);

	impl.spriteTitle_.Init(S_W - C_W * 0.475f, C_H * 0.65f, S_W * 0.8f, S_H * 1.8f);
	impl.textureTitile_.LoadImageFromStorage("asset/TEXTURE/title.png");

	impl.spritePressKey_.Init(C_W, C_H * 1.75f, S_W * 0.7f, S_H * 0.1f);
	impl.spritePressKey_.SetColor(ideaColor::SOFT_BLACK.ReplaceAlpha(0.8f));

	impl.spriteBalloon_.Init(S_W - C_W * 0.725f, C_H * 1.35f, C_W * 0.6f, C_H * 0.6f);
	impl.spriteBalloon_.SetRotation(DegreeToRadian(6));
	impl.spriteBalloon_.SetColor(ideaColor::WHITE.ReplaceAlpha(0.7f));
	impl.spriteBalloon_.SetRotation(-0.1f);

	impl.textureBalloon_.LoadImageFromStorage("asset/TEXTURE/balloon.png");

	impl.fontState_.Init(70, 200, FontReferencePoint::CENTER, "HGP創英角ﾎﾟｯﾌﾟ体");
	impl.fontState_.SetColor(ideaColor::SOFT_BLACK);

	impl.fontPressKey_.Init(50, 1000, FontReferencePoint::CENTER);

	impl.popUpBalloonCount_ = 0;
}

void TitleUI::Update()
{
	// 実装
	auto& impl = (*pImpl_);

	++impl.popUpBalloonCount_;

	float popUpRate2 = EaseInOutSine(Iteration(impl.popUpBalloonCount_, 0, STATE_TIME >> 1) * (1.0f / ((float)STATE_TIME * 0.5f)));

	// 吹き出しのリサイズ
	impl.spriteBalloon_.SetSize(-(C_W * 0.5f + (C_W * 0.055f * popUpRate2)), C_H * 0.5f + (C_H * 0.05f * popUpRate2));
}

void TitleUI::Draw()
{
	// 実装
	auto& impl = (*pImpl_);

	// シーン
	auto& scene = (*pScene_);

	// キャラクター
	auto& character = (*scene.GetGameObject<TitleCharacter>(TITLE_CHARACTER_LAYER));

	impl.spriteTitle_.DrawTexture(impl.textureTitile_);

	impl.spritePressKey_.DrawRect();

	impl.spriteBalloon_.DrawTexture(impl.textureBalloon_);

	// 状態の表示
	switch(character.GetState()){
		case TitleCharacterState::Idol:
			impl.fontState_.DrawFormatText(S_W - C_W * 0.74f, C_H * 1.325f, "        idol");
			break;
		case TitleCharacterState::Chase:
			impl.fontState_.DrawFormatText(S_W - C_W * 0.74f, C_H * 1.325f, "     chase");
			break;
		case TitleCharacterState::Escape:
			impl.fontState_.DrawFormatText(S_W - C_W * 0.75f, C_H * 1.325f, "      escape");
			break;
	}

	impl.fontPressKey_.DrawFormatText(C_W, C_H * 1.75f, "ボタンまたはENTERキーを押して開始 ");
}
