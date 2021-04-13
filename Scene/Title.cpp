#include "Title.h"
#include "Menu.h"
#include "../GameObject/TitleRenderer.h"
#include "../GameObject/TitleCharacter.h"
#include "../GameObject/TitleUI.h"

using namespace idea;

// Title�̎���
class Title::Impl{
public:
	// �R���g���[���[����
	Controller controller_;

	// �w�i
	Sprite spriteBG_;

	// �w�i�̃e�N�X�`���[
	Texture textureBG_;

	// �w�i�̃X�N���[��
	float scrollBG_ = 0.0f;

	// BGM
	Sound bgm_;

	// BGM�̃{�����[��
	float bgmVolume_ = 0.0f;

	// ���艹
	Sound seDecision_;

	// �I���t���O
	bool isEnd_ = false;
};

Title::Title() :
	pImpl_(new Title::Impl)
{
	Assert(pImpl_);
}

Title::~Title()
{
	SafeDelete(pImpl_);
}

void Title::Init()
{
	// ����
	auto& impl = (*pImpl_);

	AddGameObject<TitleRenderer>(RENDERER_LAYER);

	AddGameObject<TitleCharacter>(TITLE_CHARACTER_LAYER);

	AddGameObject<TitleUI>(TITLE_UI_LAYER);

	impl.controller_.Init(1, true);
	impl.controller_.SetConfig(PadButton::A, KeyCode::RETURN);
	impl.controller_.Update();

	impl.spriteBG_.Init(C_W, C_H, S_W, S_W);
	impl.spriteBG_.SetColor(ideaColor::SOFT_BLUE);

	impl.textureBG_.LoadImageFromFile("asset/TEXTURE/grid04.bmp");

	impl.scrollBG_ = 0.0f;

	impl.isEnd_ = false;

	impl.bgm_.LoadWaveFromStorage("2_23_AM_2.wav");
	impl.bgmVolume_ = 1.0f;
	impl.bgm_.SetVolume(impl.bgmVolume_);

	impl.seDecision_.LoadWaveFromStorage("asset/SOUND/on000.wav");
	impl.seDecision_.SetVolume(0.5f);

	impl.bgm_.Play(true);
}

void Title::Update()
{
	// ����
	auto& impl = (*pImpl_);

	// �V�[���J��
	if(fade_.IsFadeOutCompleted()){
		gameManager_.SetNextScenePtr(new Menu);
		return;
	}

	// �V�[���J�ڂɍ��킹BGM�̃{�����[����������
	if(impl.isEnd_){
		impl.bgmVolume_ -= 1.0f / 40.0f;
		impl.bgm_.SetVolume(impl.bgmVolume_);

		fade_.SetFade(40);

		return;
	}

	impl.controller_.Update();

	impl.scrollBG_ += 0.00075f;

	// �{�^�������Ńt�F�[�h
	if(!fade_.IsFading()){
		for(int i = 0; i < PadButton::BUTTON_MAX; ++i){
			if(impl.controller_.GetButton(i) == 1){
				impl.isEnd_ = true;

				fade_.SetFade(40);

				impl.seDecision_.Play();

				break;
			}
		}
	}

	Scene::Update();
}

void Title::Draw()
{
	// ����
	auto& impl = (*pImpl_);

	impl.spriteBG_.DrawDelimitedTexture(impl.textureBG_, impl.scrollBG_ * 2.0f, -impl.scrollBG_ * 1.5f, 5.0f, 5.0f);

	Scene::Draw();
}
