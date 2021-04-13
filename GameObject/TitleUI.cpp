#include "TitleUI.h"
#include "TitleCharacter.h"

using namespace idea;

class TitleUI::Impl{
public:
	// �^�C�g�����S�̃X�v���C�g
	Sprite spriteTitle_;

	// �^�C�g�����S�̃e�N�X�`���[
	Texture textureTitile_;

	// �L�[���͂𑣂��t�H���g�̔w�i
	Sprite spritePressKey_;

	// �����o���̃X�v���C�g
	Sprite spriteBalloon_;

	// �����o���̃e�N�X�`���[
	Texture textureBalloon_;

	// �����o���̓����̃J�E���g
	int popUpBalloonCount_ = 0;

	// ��ԕ\���p�t�H���g
	Font fontState_;

	// �L�[���͂𑣂��t�H���g
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
	// ����
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

	impl.fontState_.Init(70, 200, FontReferencePoint::CENTER, "HGP�n�p�p�߯�ߑ�");
	impl.fontState_.SetColor(ideaColor::SOFT_BLACK);

	impl.fontPressKey_.Init(50, 1000, FontReferencePoint::CENTER);

	impl.popUpBalloonCount_ = 0;
}

void TitleUI::Update()
{
	// ����
	auto& impl = (*pImpl_);

	++impl.popUpBalloonCount_;

	float popUpRate2 = EaseInOutSine(Iteration(impl.popUpBalloonCount_, 0, STATE_TIME >> 1) * (1.0f / ((float)STATE_TIME * 0.5f)));

	// �����o���̃��T�C�Y
	impl.spriteBalloon_.SetSize(-(C_W * 0.5f + (C_W * 0.055f * popUpRate2)), C_H * 0.5f + (C_H * 0.05f * popUpRate2));
}

void TitleUI::Draw()
{
	// ����
	auto& impl = (*pImpl_);

	// �V�[��
	auto& scene = (*pScene_);

	// �L�����N�^�[
	auto& character = (*scene.GetGameObject<TitleCharacter>(TITLE_CHARACTER_LAYER));

	impl.spriteTitle_.DrawTexture(impl.textureTitile_);

	impl.spritePressKey_.DrawRect();

	impl.spriteBalloon_.DrawTexture(impl.textureBalloon_);

	// ��Ԃ̕\��
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

	impl.fontPressKey_.DrawFormatText(C_W, C_H * 1.75f, "�{�^���܂���ENTER�L�[�������ĊJ�n ");
}
