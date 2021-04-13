#include "GameVoice.h"
#include "../Scene/MainGame.h"
#include "Vita.h"
#include "Judg.h"

using namespace idea;

namespace{
	// �J�n�{�C�X
	static const int START_VOICE = 0;

	// �v���C���[�ɕ߂܂������̃{�C�X
	static const int PLAYER_TOUCH_VOICE = 1;

	// �v���C���[���ŏ��ɕ߂܂������̃{�C�X
	static const int VITA_TOUCH_FIRST_VOICE = 2;

	// �v���C���[��߂܂������̃{�C�X
	static const int VITA_TOUCH_VOICE = 3;

	// vita�ǉ����̃{�C�X1
	static const int ADD_VITA_VOICE_1 = 4;

	// vita�ǉ����̃{�C�X2
	static const int ADD_VITA_VOICE_2 = 5;

	// vita�ǉ����̃{�C�X3
	static const int ADD_VITA_VOICE_3 = 6;

	// �W�c�ŏP���|����Ƃ��̃{�C�X
	static const int FLOCK_VITA_VOICE = 7;

	// �v���C���[�������̃{�C�X
	static const int PLAYER_WIN_VOICE = 8;

	// vita�������̃{�C�X
	static const int VITA_WIN_VOICE = 9;

	// ���C���Q�[���I�����̒ǉ��{�C�X
	static const int MAIN_GAME_VOICE = 10;

	// �G�f�B�b�g�Q�[���I�����̒ǉ��{�C�X
	static const int EDIT_GAME_VOICE = 11;

	// �{�C�X�̗݌v��
	static const int VOICE_MAX = 12;
};

class GameVoice::Impl{
public:
	// �{�C�X
	Sound voice_[VOICE_MAX];

	// �v���C���[��߂܂����̂��ŏ���
	bool vitaTouchFirst_ = false;

	// �W�c�ŏP���Ă��邩
	bool flockFlag_ = false;

	// �I���J�E���g
	int finishCount_ = 0;

	// �l�ۊǌ�
	Memory memory_;
};

GameVoice::GameVoice() :
	pImpl_(new GameVoice::Impl)
{
	Assert(pImpl_);
}

GameVoice::~GameVoice()
{
	SafeDelete(pImpl_);
}

void GameVoice::Init()
{
	// ����
	auto& impl = (*pImpl_);

	impl.voice_[START_VOICE].LoadWaveFromStorage("asset/SOUND/voice_start.wav");
	impl.voice_[PLAYER_TOUCH_VOICE].LoadWaveFromStorage("asset/SOUND/voice_player_touch.wav");
	impl.voice_[VITA_TOUCH_FIRST_VOICE].LoadWaveFromStorage("asset/SOUND/voice_vita_touch_first.wav");
	impl.voice_[VITA_TOUCH_VOICE].LoadWaveFromStorage("asset/SOUND/voice_vita_touch.wav");
	impl.voice_[ADD_VITA_VOICE_1].LoadWaveFromStorage("asset/SOUND/voice_add_vita.wav");
	impl.voice_[ADD_VITA_VOICE_2].LoadWaveFromStorage("asset/SOUND/voice_add_vita2.wav");
	impl.voice_[ADD_VITA_VOICE_3].LoadWaveFromStorage("asset/SOUND/voice_add_vita3.wav");
	impl.voice_[FLOCK_VITA_VOICE].LoadWaveFromStorage("asset/SOUND/voice_flock_vita.wav");
	impl.voice_[PLAYER_WIN_VOICE].LoadWaveFromStorage("asset/SOUND/voice_player_win.wav");
	impl.voice_[VITA_WIN_VOICE].LoadWaveFromStorage("asset/SOUND/voice_vita_win.wav");
	impl.voice_[MAIN_GAME_VOICE].LoadWaveFromStorage("asset/SOUND/voice_main_game_finish.wav");
	impl.voice_[EDIT_GAME_VOICE].LoadWaveFromStorage("asset/SOUND/voice_edit_game_finish.wav");

	for(int i = 0; i < VOICE_MAX; ++i){
		impl.voice_[i].SetVolume(4.0f);
	}

	impl.vitaTouchFirst_ = true;

	impl.flockFlag_ = false;

	impl.finishCount_ = 0;
}

void GameVoice::Update()
{
	// ����
	auto& impl = (*pImpl_);

	// �V�[��
	auto& scene = (*pScene_);

	// vita�{��
	auto& vita = (*scene.GetGameObject<Vita>(VITA_LAYER));

	// �W�c�ŏP���|�����Ă���Ƃ�
	if(impl.flockFlag_ && vita.GetState() != VitaState::Count){
		StopAll();

		impl.voice_[FLOCK_VITA_VOICE].Play();

		impl.flockFlag_ = false;
	}

	// �Q�[���I����
	if(impl.finishCount_ > 0){
		++impl.finishCount_;
	}

	if(impl.finishCount_ > 3 * 60){
		if(!impl.voice_[PLAYER_WIN_VOICE].IsPlaying()
		   && !impl.voice_[VITA_WIN_VOICE].IsPlaying()){
			StopAll();

			// �Q�[�����[�h�ɂ���Ēǉ��{�C�X��ς���
			if(impl.memory_.GetInt("game_mode") == MODE_ID_MAIN_GAME){
				impl.voice_[MAIN_GAME_VOICE].Play();
			} else{
				impl.voice_[EDIT_GAME_VOICE].Play();
			}

			impl.finishCount_ = false;
		}
	}
}

void GameVoice::Start()
{
	// ����
	auto& impl = (*pImpl_);

	StopAll();

	impl.voice_[START_VOICE].Play();
}

void GameVoice::PlayerTouch()
{
	// ����
	auto& impl = (*pImpl_);

	StopAll();

	impl.voice_[PLAYER_TOUCH_VOICE].Play();
}

void GameVoice::VitaTouch()
{
	// ����
	auto& impl = (*pImpl_);

	StopAll();

	// �߂܂����̂��ŏ����ǂ����ŕ���
	if(impl.vitaTouchFirst_){
		impl.voice_[VITA_TOUCH_FIRST_VOICE].Play();
		impl.vitaTouchFirst_ = false;
	} else{
		impl.voice_[VITA_TOUCH_VOICE].Play();
	}
}

void GameVoice::AddVita(int vitaNum)
{
	// ����
	auto& impl = (*pImpl_);

	StopAll();

	// vita�̐��ŕ���
	if(vitaNum >= 7){
		impl.voice_[ADD_VITA_VOICE_3].Play();
	}else if(vitaNum % 2 == 0){
		impl.voice_[ADD_VITA_VOICE_1].Play();
	} else{
		impl.voice_[ADD_VITA_VOICE_2].Play();
	}
}

void GameVoice::FlockVita()
{
	// ����
	auto& impl = (*pImpl_);

	impl.flockFlag_ = true;
}

void GameVoice::PlayerWin()
{
	// ����
	auto& impl = (*pImpl_);

	// �J�E���g�J�n
	impl.finishCount_ = 1;

	StopAll();

	impl.voice_[PLAYER_WIN_VOICE].Play();
}

void GameVoice::VitaWin()
{
	// ����
	auto& impl = (*pImpl_);

	// �J�E���g�J�n
	impl.finishCount_ = 1;

	StopAll();

	impl.voice_[VITA_WIN_VOICE].Play();
}

void GameVoice::StopAll()
{
	// ����
	auto& impl = (*pImpl_);

	for(int i = 0; i < VOICE_MAX; ++i){
		impl.voice_[i].Stop();
	}
}
