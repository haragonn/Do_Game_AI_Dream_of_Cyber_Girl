#include "Tutorial.h"
#include "MainGame.h"

using namespace idea;

// Tutorial�̎���
class Tutorial::Impl{
public:
	// �R���g���[���[����
	Controller controller_;

	// �w�i
	Sprite spriteBG_;

	// �w�i�̃e�N�X�`���[
	Texture textureBG_;

	// �������
	Sprite spriteConfig_;

	// ��������̃e�N�X�`���[
	Texture textureConfig_;

	// �L�[���͂𑣂��t�H���g�̔w�i
	Sprite spritePressKey_;

	// �L�[���͂𑣂��t�H���g
	Font fontPressKey_;

	// ��������t�H���g
	Font fontInfo_;

	// ���艹
	Sound seDecision_;

	// �w�i�̃X�N���[��
	float scrollBG_ = 0.0f;

	// �I���t���O
	bool isEnd_ = false;
};

Tutorial::Tutorial() :
	pImpl_(new Tutorial::Impl)
{
	Assert(pImpl_);
}

Tutorial::~Tutorial()
{
	SafeDelete(pImpl_);
}

void Tutorial::Init()
{
	// ����
	auto& impl = (*pImpl_);

	impl.controller_.Init(1, true);
	impl.controller_.SetConfig(PadButton::A, KeyCode::RETURN);
	impl.controller_.SetConfig(PadButton::START, KeyCode::SPACE);
	impl.controller_.Update();

	impl.spriteBG_.Init(C_W, C_H, S_W, S_W);

	impl.textureBG_.LoadImageFromStorage("asset/TEXTURE/grid00.bmp");

	impl.spriteConfig_.Init(C_W, C_H * 0.85f, 1000, 600);

	impl.textureConfig_.LoadImageFromStorage("asset/TEXTURE/config.png");

	impl.scrollBG_ = 0.0f;

	impl.spritePressKey_.Init(C_W, C_H * 1.8f, S_W * 0.7f, S_H * 0.1f);
	impl.spritePressKey_.SetColor(ideaColor::SOFT_BLACK.ReplaceAlpha(0.8f));

	impl.seDecision_.LoadWaveFromStorage("asset/SOUND/on000.wav");
	impl.seDecision_.SetVolume(0.5f);

	impl.fontInfo_.Init(72, 1000);
	impl.fontInfo_.SetColor(ideaColor::SOFT_BLACK);

	impl.fontPressKey_.Init(50, 1000, FontReferencePoint::CENTER);

	impl.isEnd_ = false;
}

void Tutorial::Update()
{
	// ����
	auto& impl = (*pImpl_);

	// �V�[���J��
	if(fade_.IsFadeOutCompleted()){
		gameManager_.SetNextScenePtr(new MainGame);
		return;
	}

	impl.controller_.Update();

	// �V�[���J�ڂɍ��킹BGM�̃{�����[����������
	if(impl.isEnd_){
		fade_.SetFade(40);

		return;
	}

	// �{�^�������Ńt�F�[�h
	if(!fade_.IsFading()){
		if(impl.controller_.IsKeyPress(KeyCode::RETURN)){
			impl.isEnd_ = true;

			fade_.SetFade(40);

			impl.seDecision_.Play();
		}

		for(int i = 0; i < PadButton::BUTTON_MAX; ++i){
			if(impl.controller_.GetButton(i) == 1){
				impl.isEnd_ = true;

				fade_.SetFade(40);

				impl.seDecision_.Play();

				break;
			}
		}
	}

	impl.scrollBG_ += 0.00075f;
}

void Tutorial::Draw()
{
	// ����
	auto& impl = (*pImpl_);

	// �w�i
	impl.spriteBG_.SetColor(ideaColor::WHITE);
	impl.spriteBG_.DrawDelimitedTexture(impl.textureBG_, impl.scrollBG_ * 2.0f, -impl.scrollBG_ * 1.5f, 5.0f, 5.0f);

	// ������@�\��
	impl.spriteConfig_.DrawTexture(impl.textureConfig_);

	// �S�̂������Â�����
	impl.spriteBG_.SetColor(ideaColor::SOFT_BLACK.ReplaceAlpha(0.1f));
	impl.spriteBG_.DrawRect();

	// �L�[���͂𑣂�
	impl.spritePressKey_.DrawRect();
	impl.fontPressKey_.DrawFormatText(C_W, C_H * 1.8f, "�{�^���܂���ENTER�L�[�������ĊJ�n ");

	// �������
	impl.fontInfo_.DrawFormatText(0.0f, 0.0f, "�������");
}
