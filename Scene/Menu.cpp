#include "Menu.h"
#include "Tutorial.h"
#include "EditMode.h"

using namespace idea;

// ���[�hID
namespace{
	// ���C���Q�[��
	static const int MAIN_GAME = 0;

	// �G�f�B�b�g�Q�[��
	static const int EDIT_GAME = 1;

	// �G�f�B�b�g���[�h
	static const int EDIT_MODE = 2;
	
	// �I��
	static const int EXIT = 3;
}

// Menu�̎���
class Menu::Impl{
public:
	// �R���g���[���[����
	Controller controller_;

	// �}�E�X����
	Mouse mouse_;

	// �I���{�^��
	ButtonSprite button_[4];

	// vita�̃��f��
	PmxModel vitaModel_;

	// �ҋ@���[�V����
	VmdMotion motionIdol_;

	// �J����
	WrapCamera camera_;

	// �w�i
	Sprite spriteBG_;

	// �w�i�̃e�N�X�`���[
	Texture textureBG_;

	// �������w�i
	Sprite spriteInfo_;

	// �L�[�����q�[
	Sprite spriteKey_;

	// �V�[���\���p�t�H���g
	Font fontScene_;

	// �������\���p�t�H���g
	Font fontInfo_;

	// �V�[���I��
	int selectScene_ = 0;

	// �w�i�̃X�N���[��
	float scrollBG_ = 0.0f;

	// BGM
	Sound bgm_;

	// BGM�̃{�����[��
	float bgmVolume_ = 0.0f;

	// �I����
	Sound seSelect_;

	// ���艹
	Sound seDecision_;

	// �I����
	Sound seOff_;

	// �����{�C�X
	Sound voiceInfo_[4];

	// �I���t���O
	bool isEnd_ = false;
};

Menu::Menu() :
	pImpl_(new Menu::Impl)
{
	Assert(pImpl_);
}

Menu::~Menu()
{
	SafeDelete(pImpl_);
}

void Menu::Init()
{
	// ����
	auto& impl = (*pImpl_);

	impl.controller_.Init(1, true);
	impl.controller_.SetConfig(PadButton::A, KeyCode::RETURN);
	impl.controller_.Update();

	impl.button_[MAIN_GAME].Init(S_W * 0.75f, S_H * 0.15f, S_W * 0.3f, S_H * 0.15f);
	impl.button_[EDIT_GAME].Init(S_W * 0.75f, S_H * 0.35f, S_W * 0.3f, S_H * 0.15f);
	impl.button_[EDIT_MODE].Init(S_W * 0.75f, S_H * 0.55f, S_W * 0.3f, S_H * 0.15f);
	impl.button_[EXIT].Init(S_W * 0.75f, S_H * 0.75f, S_W * 0.3f, S_H * 0.15f);

	for(int i = 0; i < 4; ++i){
		impl.button_[i].SetColor(ideaColor::SOFT_BLACK);
	}

	impl.camera_.Init(DegreeToRadian(60), S_W / S_H, 0.1f, 100.0f, 13.0f);
	impl.camera_.SetFocus(0.0f, 0.0f, 0.0f);
	impl.camera_.SetRotation(DegreeToRadian(0), DegreeToRadian(10));
	impl.camera_.SetViewPort(0.0f, 0.0f, S_W, S_H);

	impl.vitaModel_.Init(-5.0f, -17.5f, 0.0f);
	impl.vitaModel_.SetScale(1.0f, 1.0f, 1.0f);
	impl.vitaModel_.SetRotation(0.0f, DegreeToRadian(-20), 0.0f);
	impl.vitaModel_.LoadPmxMeshFromStorage("asset/MODEL/Vita/Vita.pmx");
	impl.camera_.AddObject(impl.vitaModel_);

	impl.motionIdol_.LoadVmdMotionFromStorage("asset/MOTION/_�ҋ@.vmd");

	impl.spriteBG_.Init(C_W, C_H, S_W, S_W);
	impl.spriteBG_.SetColor(ideaColor::CYAN.ReplaceRed(0.5f));

	impl.textureBG_.LoadImageFromStorage("asset/TEXTURE/grid04.bmp");

	impl.spriteInfo_.Init(S_W * 0.275f, S_H * 0.725f, S_W * 0.45f, S_H * 0.4f);
	impl.spriteInfo_.SetColor(ideaColor::SOFT_BLACK);

	impl.spriteKey_.Init(S_W * 0.75f, S_H * 0.9f, S_W * 0.4f, C_H * 0.2f);
	impl.spriteKey_.SetColor(ideaColor::SOFT_BLACK.ReplaceAlpha(0.8f));

	impl.fontScene_.Init(40, 1000, FontReferencePoint::CENTER);

	impl.fontInfo_.Init(40, 500, FontReferencePoint::CENTER);

	impl.selectScene_ = 0;

	impl.scrollBG_ = 0.0f;

	impl.isEnd_ = false;

	impl.bgm_.LoadWaveFromStorage("SUMMER_TRIANGLE_2.wav");
	impl.bgmVolume_ = 1.0f;
	impl.bgm_.SetVolume(impl.bgmVolume_);

	impl.seSelect_.LoadWaveFromStorage("asset/SOUND/po000.wav");
	impl.seSelect_.SetVolume(0.5f);
	impl.seDecision_.LoadWaveFromStorage("asset/SOUND/on000.wav");
	impl.seDecision_.SetVolume(0.5f);
	impl.seOff_.LoadWaveFromStorage("asset/SOUND/off000.wav");
	impl.seOff_.SetVolume(0.5f);

	impl.voiceInfo_[0].LoadWaveFromStorage("asset/SOUND/voice_main_geme.wav");
	impl.voiceInfo_[1].LoadWaveFromStorage("asset/SOUND/voice_edit_game.wav");
	impl.voiceInfo_[2].LoadWaveFromStorage("asset/SOUND/voice_edit_mode.wav");
	impl.voiceInfo_[3].LoadWaveFromStorage("asset/SOUND/voice_exit.wav");

	for(int i = 0; i < 4; ++i){
		impl.voiceInfo_[i].SetVolume(4.0f);
	}

	impl.bgm_.Play(true);
	impl.voiceInfo_[0].Play();
}

void Menu::UnInit()
{
}

void Menu::Update()
{
	// ����
	auto& impl = (*pImpl_);

	// �V�[���J��
	if(fade_.IsFadeOutCompleted()){
		switch(impl.selectScene_){
			case MAIN_GAME:
				memory_.SetInt("game_mode", MODE_ID_MAIN_GAME);
				gameManager_.SetNextScenePtr(new Tutorial);
				break;

			case EDIT_GAME:
				memory_.SetInt("game_mode", MODE_ID_EDIT_GAME);
				gameManager_.SetNextScenePtr(new Tutorial);
				break;

			case EDIT_MODE:
				gameManager_.SetNextScenePtr(new EditMode);
				break;

			case EXIT:
				gameManager_.SetNextScenePtr(nullptr);
				break;
		}

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

	// �{�^�������Ńt�F�[�h
	if(!fade_.IsFading()
		&& impl.controller_.GetButton(PadButton::A) == 1){
		impl.isEnd_ = true;

		fade_.SetFade(40);

		if(impl.selectScene_ != EXIT){
			impl.seDecision_.Play();
		} else{
			impl.seOff_.Play();
		}
	}

	// �V�[���I��
	if(impl.controller_.GetUp() == 1){
		impl.selectScene_ = Wrap(impl.selectScene_ - 1, MAIN_GAME, EXIT);

		impl.seSelect_.Play();
		for(int i = 0; i < 4; ++i){
			impl.voiceInfo_[i].Stop();
		}
		impl.voiceInfo_[impl.selectScene_].Play();
	} else if(impl.controller_.GetDown() == 1){
		impl.selectScene_ = Wrap(impl.selectScene_ + 1, MAIN_GAME, EXIT);

		impl.seSelect_.Play();
		for(int i = 0; i < 4; ++i){
			impl.voiceInfo_[i].Stop();
		}
		impl.voiceInfo_[impl.selectScene_].Play();
	}

	impl.vitaModel_.UpdateVmd(impl.motionIdol_, 2.0f, 0.1f, true, false);

	impl.scrollBG_ += 0.00075f;
}

void Menu::Draw()
{
	// ����
	auto& impl = (*pImpl_);

	// �w�i
	impl.spriteBG_.DrawDelimitedTexture(impl.textureBG_, impl.scrollBG_ * 2.0f, -impl.scrollBG_ * 1.5f, 5.0f, 5.0f);

	// ���f���\��
	impl.camera_.DrawObject();

	// �{�^���\��
	// �g�̕���
	for(int i = 0; i < 4; ++i){
		impl.button_[i].SetSize(S_W * 0.3f + 10.0f, S_H * 0.15f + 10.0f);
		impl.button_[i].SetColor(ideaColor::WHITE);
		impl.button_[i].DrawRect();
	}

	// ���̕���
	for(int i = 0; i < 4; ++i){
		impl.button_[i].SetSize(S_W * 0.3f, S_H * 0.15f);
		impl.button_[i].SetColor(ideaColor::SOFT_BLACK);
	}

	// �I������Ă���{�^���͉��F��
	impl.button_[impl.selectScene_].SetColor(ideaColor::SOFT_YELLOW);

	for(int i = 0; i < 4; ++i){
		impl.button_[i].DrawRect();
	}

	// �{�^�������`��,�I������Ă��镶���͔���
	if(impl.selectScene_ == MAIN_GAME){
		impl.fontScene_.SetColor(ideaColor::SOFT_BLACK);
	} else{
		impl.fontScene_.SetColor(ideaColor::WHITE);
	}
	impl.fontScene_.DrawFormatText(S_W * 0.75f, S_H * 0.15f, "Main Stage Game");

	if(impl.selectScene_ == EDIT_GAME){
		impl.fontScene_.SetColor(ideaColor::SOFT_BLACK);
	} else{
		impl.fontScene_.SetColor(ideaColor::WHITE);
	}
	impl.fontScene_.DrawFormatText(S_W * 0.75f, S_H * 0.35f, "Edit Stage Game");

	if(impl.selectScene_ == EDIT_MODE){
		impl.fontScene_.SetColor(ideaColor::SOFT_BLACK);
	} else{
		impl.fontScene_.SetColor(ideaColor::WHITE);
	}
	impl.fontScene_.DrawFormatText(S_W * 0.75f, S_H * 0.55f, "Stage Edit Mode");

	if(impl.selectScene_ == EXIT){
		impl.fontScene_.SetColor(ideaColor::SOFT_BLACK);
	} else{
		impl.fontScene_.SetColor(ideaColor::WHITE);
	}
	impl.fontScene_.DrawFormatText(S_W * 0.75f, S_H * 0.75f, "Exit");
	
	// �L�[����
	impl.spriteKey_.DrawRect();
	impl.fontInfo_.DrawFormatText(S_W * 0.75f, S_H * 0.9f, "A�{�^����ENTER�L�[�Ō���");

	// ������
	// �g�̕���
	impl.spriteInfo_.SetSize(S_W * 0.45f + 10.0f, S_H * 0.4f + 10.0f);
	impl.spriteInfo_.SetColor(ideaColor::WHITE);
	impl.spriteInfo_.DrawRect();

	// ���̕���
	impl.spriteInfo_.SetSize(S_W * 0.45f, S_H * 0.4f);
	impl.spriteInfo_.SetColor(ideaColor::SOFT_BLACK);
	impl.spriteInfo_.DrawRect();

	// ���b�Z�[�W
	switch(impl.selectScene_){
		case MAIN_GAME:
			impl.fontInfo_.DrawFormatText(S_W * 0.275f, S_H * 0.6f,   "���炩���ߗp�ӂ��ꂽ�X�e�[�W");
			impl.fontInfo_.DrawFormatText(S_W * 0.275f, S_H * 0.675f, "�ŗV�ׂ܂���I              ");
			impl.fontInfo_.DrawFormatText(S_W * 0.275f, S_H * 0.75f,  "���ƋS�������ɋ����܂��傤�I");
			break;

		case EDIT_GAME:
			impl.fontInfo_.DrawFormatText(S_W * 0.275f, S_H * 0.6f,   "�����ō쐬�����X�e�[�W      ");
			impl.fontInfo_.DrawFormatText(S_W * 0.275f, S_H * 0.675f, "�ŗV�ׂ܂���I              ");
			impl.fontInfo_.DrawFormatText(S_W * 0.275f, S_H * 0.75f,  "���ƋS�������ɋ����܂��傤�I");
			break;

		case EDIT_MODE:
			impl.fontInfo_.DrawFormatText(S_W * 0.275f, S_H * 0.6f,   "�X�e�[�W���쐬���郂�[�h�ł�");
			impl.fontInfo_.DrawFormatText(S_W * 0.275f, S_H * 0.675f, "�쐬�����X�e�[�W�͍ŒZ�o�H��");
			impl.fontInfo_.DrawFormatText(S_W * 0.275f, S_H * 0.75f,  "�x�C�N����Edit Stage Game�� ");
			impl.fontInfo_.DrawFormatText(S_W * 0.275f, S_H * 0.825f, "�V�ׂ�悤�ɂȂ�܂���I    ");
			break;

		case EXIT:
			impl.fontInfo_.DrawFormatText(S_W * 0.275f, S_H * 0.6f,   "���Ȃ��I�����܂�        ");
			impl.fontInfo_.DrawFormatText(S_W * 0.275f, S_H * 0.675f, "�c�c�����A�����Ⴄ��ł����H");
			break;
	}
}