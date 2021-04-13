#include "MainGame.h"
#include "Menu.h"
#include "../GameObject/Renderer.h"
#include "../GameObject/MetaAI.h"
#include "../GameObject/Player.h"
#include "../GameObject/Vita.h"
#include "../GameObject/Item.h"
#include "../GameObject/Judg.h"
#include "../GameObject/GameUI.h"
#include "../GameObject/Map.h"
#include "../GameObject/Collider.h"
#include "../GameObject/GameVoice.h"

using namespace idea;

// MainGame�̎���
class MainGame::Impl{
public:
	// �R���g���[���[����
	Controller controller_;

	// �v���C���[���S��
	bool isPlayerOni_ = false;

	// �Q�[���J�n���̃J�E���g
	int introCount_ = 0;

	// �Q�[���J�n���\���t�H���g
	Font fontIntro_;

	// �I���J�E���g
	int finishCount_ = 0;

	// �I�����\���t�H���g
	Font fontFinish_;

	// �L�[���͂𑣂��t�H���g�̔w�i
	Sprite spritePressKey_;

	// �L�[���͂𑣂��t�H���g
	Font fontPressKey_;

	// BGM
	Sound bgm_;

	// BGM�̃{�����[��
	float bgmVolume_ = 0.0f;

	// �|�[�Y����
	bool isPause_ = false;

	// �|�[�Y���̑I��
	int pauseSelect_ = 0;

	// �|�[�Y��ʂ̘g
	Sprite spritePause_;

	// �|�[�Y��ʂ̑I���A�C�R��
	Sprite spritePauseSelect_;

	// �|�[�Y��ʂ̑I���A�C�R���̃e�N�X�`���[
	Texture texturePauseSelect_[3];

	// �|�[�Y��ʂ̑I���A�C�R���̊p�x
	float selectRad_[2] = { 0.0f, 0.0f };

	// �|�[�Y��ʂ̃t�H���g
	Font fontPause_;

	// �|�[�Y�N����
	Sound sePauseIn_;

	// �|�[�Y������
	Sound sePauseOut_;

	// �I����
	Sound seSelect_;

	// �I���t���O
	bool isEnd_ = false;

	// �X�e���V���}�X�N
	Mask mask_;

	// �f�o�b�O���O
	Debug debugLog_;
};

MainGame::MainGame()
	: pImpl_(new MainGame::Impl)
{
	Assert(pImpl_);
}

MainGame::~MainGame()
{
	SafeDelete(pImpl_);
}

void MainGame::Init()
{
	// ����
	auto& impl = (*pImpl_);

	AddGameObject<Renderer>(RENDERER_LAYER);

	AddGameObject<Map>(MAP_LAYER);

	AddGameObject<MetaAI>(META_AI_LAYER);

	AddGameObject<Judg>(JUDG_LAYER);

	AddGameObject<GameUI>(GAME_UI_LAYER);

	AddGameObject<Collider>(COLLIDER_LAYER);

	AddGameObject<GameVoice>(GAME_VOICE_LAYER)->Start();

	impl.controller_.Init(1, true);
	impl.controller_.SetConfig(PadButton::A, KeyCode::RETURN);
	impl.controller_.SetConfig(PadButton::START, KeyCode::SPACE);

	impl.isPlayerOni_ = true;

	impl.introCount_ = 0;
	impl.fontIntro_.Init(200, 600, FontReferencePoint::CENTER);

	impl.finishCount_ = 0;
	impl.fontFinish_.Init(250, 600, FontReferencePoint::CENTER);

	impl.spritePressKey_.Init(C_W, C_H * 1.5f, S_W * 0.8f, S_H * 0.1f);
	impl.spritePressKey_.SetColor(ideaColor::SOFT_BLACK.ReplaceAlpha(0.8f));
	impl.fontPressKey_.Init(60, 400, FontReferencePoint::CENTER);

	impl.isPause_ = false;
	impl.pauseSelect_ = 0;

	impl.spritePause_.Init(C_W, C_H, C_W, C_H);
	impl.spritePause_.SetColor(ideaColor::SOFT_BLACK);

	impl.spritePauseSelect_.Init(C_W * 0.6f, C_H * 0.95f, 70.0f, 70.0f);
	impl.texturePauseSelect_[0].LoadImageFromFile("asset/TEXTURE/ring0.png");
	impl.texturePauseSelect_[1].LoadImageFromFile("asset/TEXTURE/ring1.png");
	impl.texturePauseSelect_[2].LoadImageFromFile("asset/TEXTURE/ring2.png");

	impl.selectRad_[0] = 0.0f;
	impl.selectRad_[1] = 0.0f;

	impl.fontPause_.Init(60, 500, FontReferencePoint::CENTER);

	impl.bgm_.LoadWaveFromStorage("�ɓ��̗r�A�e���L���X�^�[�Ɨx��_2.wav");
	impl.bgmVolume_ = 1.0f;
	impl.bgm_.SetVolume(impl.bgmVolume_);

	impl.sePauseIn_.LoadWaveFromStorage("asset/SOUND/on000.wav");
	impl.sePauseIn_.SetVolume(0.5f);
	impl.sePauseOut_.LoadWaveFromStorage("asset/SOUND/off000.wav");
	impl.sePauseOut_.SetVolume(0.5f);
	impl.seSelect_.LoadWaveFromStorage("asset/SOUND/po000.wav");
	impl.seSelect_.SetVolume(0.5f);

	impl.isEnd_ = false;

	impl.bgm_.Play(true);

	// �ʒu�����p�X�V
	Scene::Update();
	impl.debugLog_.ClearTrace();
	impl.debugLog_.ClearLog();

	Scene::Update();
}

void MainGame::UnInit()
{
	Scene::UnInit();
}

void MainGame::Update()
{
	// ����
	auto& impl = (*pImpl_);

	// ���s���
	auto& judg = (*GetGameObject<Judg>(JUDG_LAYER));

	impl.controller_.Update();

	// �|�[�Y��ʃA�C�R����]
	impl.selectRad_[0] -= 0.005f;
	impl.selectRad_[1] += 0.005f;

	// �V�[���J��
	if(fade_.IsFadeOutCompleted()){
		impl.debugLog_.ClearTrace();
		impl.debugLog_.ClearLog();
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

	// �Q�[���I����{�^�������Ńt�F�[�h
	if(impl.finishCount_ > 80
	   && !fade_.IsFading()){
		for(int i = 0; i < PadButton::BUTTON_MAX; ++i){
			if(impl.controller_.GetButton(i) == 1){
				impl.isEnd_ = true;

				fade_.SetFade(40);

				// �|�[�Y���g���܂킷
				impl.sePauseIn_.Play();

				break;
			}
		}
	}

	// ����������
	if(judg.GetWinner()){
		++impl.finishCount_;

		impl.fontPressKey_.SetColor(ideaColor::WHITE.ReplaceAlpha(EaseInOutCubic(Iteration(impl.finishCount_ - 80, 0, 90) / 90.0f) + 0.2f));

		GetGameObject<GameVoice>(GAME_VOICE_LAYER)->Update();

		return;
	}

	// �C���g�����o
	if(impl.introCount_ < 200){
		++impl.introCount_;

		return;
	}

	// �|�[�Y
	if(impl.controller_.GetButton(PadButton::START) == 1){
		impl.isPause_ = !impl.isPause_;
		impl.pauseSelect_ = 0;

		if(impl.isPause_){
			impl.sePauseIn_.Play();
		} else{
			impl.sePauseOut_.Play();
		}
	}

	// �|�[�Y������
	if(impl.isPause_){
		if(impl.controller_.GetUp() == 1){
			impl.pauseSelect_ = Wrap(impl.pauseSelect_ - 1, 0, 1);
			impl.seSelect_.Play();
		} else if(impl.controller_.GetDown() == 1){
			impl.pauseSelect_ = Wrap(impl.pauseSelect_ + 1, 0, 1);
			impl.seSelect_.Play();
		}

		if(impl.controller_.GetButton(PadButton::A) == 1){
			if(impl.pauseSelect_ == 0){
				impl.isPause_ = false;
				impl.sePauseOut_.Play();
			} else{
				fade_.SetFade(40);
				impl.sePauseOut_.Play();
			}
		}

		return;
	}

	impl.debugLog_.ClearTrace();
	impl.debugLog_.ClearLog();

	Scene::Update();

	// vita�̏�Ԃ��f�o�b�O���ɏ�������
	auto& vita = (*GetGameObject<Vita>(VITA_LAYER));
	switch(vita.GetState()){
		case VitaState::Idol:
			impl.debugLog_.Log("AI State�@�@�@�@�FIdol");
			break;
		case VitaState::Chase:
			impl.debugLog_.Log("AI State�@�@�@�@�FChase");
			break;
		case VitaState::StopBy:
			impl.debugLog_.Log("AI State�@�@�@�@�FStopBy");
			break;
		case VitaState::Escape:
			impl.debugLog_.Log("AI State�@�@�@�@�FEscape");
			break;
		case VitaState::Count:
			impl.debugLog_.Log("AI State�@�@�@�@�FCount");
			break;
		case VitaState::Disappearance:
			impl.debugLog_.Log("AI State�@�@�@�@�FDisappearance");
			break;
	}
}

void MainGame::Draw()
{
	// ����
	auto& impl = (*pImpl_);

	Scene::Draw();

	// �C���g�����o
	if(impl.introCount_ < 100){
		impl.fontIntro_.DrawFormatText(C_W, C_H * 0.9f - C_H * 1.2f * (1.0f - EaseOutBounce((impl.introCount_ / 80.0f))), "��[��");
	} else if(impl.introCount_ < 200){
		impl.fontIntro_.DrawFormatText(C_W, C_H * 0.9f - C_H * 1.2f * (1.0f - EaseOutBounce(((impl.introCount_ - 100) / 80.0f))), "�X�^�[�g�I");
	}

	// ���s���
	auto& judg = (*GetGameObject<Judg>(JUDG_LAYER));

	// �������o
	if(judg.GetWinner() == PLAYER_WIN){
		impl.fontFinish_.DrawFormatText(C_W, C_H * 0.9f - C_H * 1.2f * (1.0f - EaseOutBounce(impl.finishCount_ / 80.0f)), "WIN!");
	} else if(judg.GetWinner() == VITA_WIN){
		impl.fontFinish_.DrawFormatText(C_W, C_H * 0.9f - C_H * 1.2f * (1.0f - EaseOutBounce(impl.finishCount_ / 80.0f)), "LOSE");
	}

	if(impl.finishCount_ > 80){
		impl.spritePressKey_.DrawRect();
		impl.fontPressKey_.DrawFormatText(C_W, C_H * 1.5f, "�{�^���܂���ENTER�L�[�������ďI��");
	}

	// �|�[�Y���
	if(impl.isPause_){
		impl.mask_.DrawMask();
		impl.spritePause_.Init(C_W, C_H, C_W, C_H);
		impl.spritePause_.DrawRect();
		impl.mask_.DrawXor();
		impl.spritePause_.Init(C_W, C_H, C_W + 20.0f, C_H + 20.0f);
		impl.spritePause_.SetColor(ideaColor::WHITE.ReplaceAlpha(0.8f));
		impl.spritePause_.DrawRect();
		impl.mask_.EndMask();

		impl.spritePause_.Init(C_W, C_H, C_W, C_H);
		impl.spritePause_.SetColor(ideaColor::SOFT_BLACK.ReplaceAlpha(0.7f));
		impl.spritePause_.DrawRect();

		impl.fontPause_.DrawFormatText(C_W, C_H * 0.65f, "�ꎞ��~");
		impl.fontPause_.DrawFormatText(C_W, C_H * 0.95f, "�ĊJ");
		impl.fontPause_.DrawFormatText(C_W, C_H * 1.25f, "Menu�ɖ߂�");

		if(impl.pauseSelect_ == 0){
			impl.spritePauseSelect_.Init(C_W * 0.8f, C_H * 0.95f, 70.0f, 70.0f);
			impl.spritePauseSelect_.SetRotation(0.0f);
			impl.spritePauseSelect_.DrawTexture(impl.texturePauseSelect_[0]);
			impl.spritePauseSelect_.SetRotation(impl.selectRad_[0]);
			impl.spritePauseSelect_.DrawTexture(impl.texturePauseSelect_[1]);
			impl.spritePauseSelect_.SetRotation(impl.selectRad_[1]);
			impl.spritePauseSelect_.DrawTexture(impl.texturePauseSelect_[2]);
			impl.spritePauseSelect_.Init(C_W * 1.2f, C_H * 0.95f, 70.0f, 70.0f);
			impl.spritePauseSelect_.SetRotation(0.0f);
			impl.spritePauseSelect_.DrawTexture(impl.texturePauseSelect_[0]);
			impl.spritePauseSelect_.SetRotation(impl.selectRad_[0]);
			impl.spritePauseSelect_.DrawTexture(impl.texturePauseSelect_[1]);
			impl.spritePauseSelect_.SetRotation(impl.selectRad_[1]);
			impl.spritePauseSelect_.DrawTexture(impl.texturePauseSelect_[2]);
		} else{
			impl.spritePauseSelect_.Init(C_W * 0.65f, C_H * 1.25f, 70.0f, 70.0f);
			impl.spritePauseSelect_.SetRotation(0.0f);
			impl.spritePauseSelect_.DrawTexture(impl.texturePauseSelect_[0]);
			impl.spritePauseSelect_.SetRotation(impl.selectRad_[0]);
			impl.spritePauseSelect_.DrawTexture(impl.texturePauseSelect_[1]);
			impl.spritePauseSelect_.SetRotation(impl.selectRad_[1]);
			impl.spritePauseSelect_.DrawTexture(impl.texturePauseSelect_[2]);
			impl.spritePauseSelect_.Init(C_W * 1.35f, C_H * 1.25f, 70.0f, 70.0f);
			impl.spritePauseSelect_.SetRotation(0.0f);
			impl.spritePauseSelect_.DrawTexture(impl.texturePauseSelect_[0]);
			impl.spritePauseSelect_.SetRotation(impl.selectRad_[0]);
			impl.spritePauseSelect_.DrawTexture(impl.texturePauseSelect_[1]);
			impl.spritePauseSelect_.SetRotation(impl.selectRad_[1]);
			impl.spritePauseSelect_.DrawTexture(impl.texturePauseSelect_[2]);
		}
	}
}
