#include "GameVoice.h"
#include "../Scene/MainGame.h"
#include "Vita.h"
#include "Judg.h"

using namespace idea;

namespace{
	// 開始ボイス
	static const int START_VOICE = 0;

	// プレイヤーに捕まった時のボイス
	static const int PLAYER_TOUCH_VOICE = 1;

	// プレイヤーを最初に捕まえた時のボイス
	static const int VITA_TOUCH_FIRST_VOICE = 2;

	// プレイヤーを捕まえた時のボイス
	static const int VITA_TOUCH_VOICE = 3;

	// vita追加時のボイス1
	static const int ADD_VITA_VOICE_1 = 4;

	// vita追加時のボイス2
	static const int ADD_VITA_VOICE_2 = 5;

	// vita追加時のボイス3
	static const int ADD_VITA_VOICE_3 = 6;

	// 集団で襲い掛かるときのボイス
	static const int FLOCK_VITA_VOICE = 7;

	// プレイヤー勝利時のボイス
	static const int PLAYER_WIN_VOICE = 8;

	// vita勝利時のボイス
	static const int VITA_WIN_VOICE = 9;

	// メインゲーム終了時の追加ボイス
	static const int MAIN_GAME_VOICE = 10;

	// エディットゲーム終了時の追加ボイス
	static const int EDIT_GAME_VOICE = 11;

	// ボイスの累計数
	static const int VOICE_MAX = 12;
};

class GameVoice::Impl{
public:
	// ボイス
	Sound voice_[VOICE_MAX];

	// プレイヤーを捕まえたのが最初か
	bool vitaTouchFirst_ = false;

	// 集団で襲っているか
	bool flockFlag_ = false;

	// 終了カウント
	int finishCount_ = 0;

	// 値保管庫
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
	// 実装
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
	// 実装
	auto& impl = (*pImpl_);

	// シーン
	auto& scene = (*pScene_);

	// vita本体
	auto& vita = (*scene.GetGameObject<Vita>(VITA_LAYER));

	// 集団で襲い掛かっているとき
	if(impl.flockFlag_ && vita.GetState() != VitaState::Count){
		StopAll();

		impl.voice_[FLOCK_VITA_VOICE].Play();

		impl.flockFlag_ = false;
	}

	// ゲーム終了時
	if(impl.finishCount_ > 0){
		++impl.finishCount_;
	}

	if(impl.finishCount_ > 3 * 60){
		if(!impl.voice_[PLAYER_WIN_VOICE].IsPlaying()
		   && !impl.voice_[VITA_WIN_VOICE].IsPlaying()){
			StopAll();

			// ゲームモードによって追加ボイスを変える
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
	// 実装
	auto& impl = (*pImpl_);

	StopAll();

	impl.voice_[START_VOICE].Play();
}

void GameVoice::PlayerTouch()
{
	// 実装
	auto& impl = (*pImpl_);

	StopAll();

	impl.voice_[PLAYER_TOUCH_VOICE].Play();
}

void GameVoice::VitaTouch()
{
	// 実装
	auto& impl = (*pImpl_);

	StopAll();

	// 捕まえたのが最初かどうかで分岐
	if(impl.vitaTouchFirst_){
		impl.voice_[VITA_TOUCH_FIRST_VOICE].Play();
		impl.vitaTouchFirst_ = false;
	} else{
		impl.voice_[VITA_TOUCH_VOICE].Play();
	}
}

void GameVoice::AddVita(int vitaNum)
{
	// 実装
	auto& impl = (*pImpl_);

	StopAll();

	// vitaの数で分岐
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
	// 実装
	auto& impl = (*pImpl_);

	impl.flockFlag_ = true;
}

void GameVoice::PlayerWin()
{
	// 実装
	auto& impl = (*pImpl_);

	// カウント開始
	impl.finishCount_ = 1;

	StopAll();

	impl.voice_[PLAYER_WIN_VOICE].Play();
}

void GameVoice::VitaWin()
{
	// 実装
	auto& impl = (*pImpl_);

	// カウント開始
	impl.finishCount_ = 1;

	StopAll();

	impl.voice_[VITA_WIN_VOICE].Play();
}

void GameVoice::StopAll()
{
	// 実装
	auto& impl = (*pImpl_);

	for(int i = 0; i < VOICE_MAX; ++i){
		impl.voice_[i].Stop();
	}
}
