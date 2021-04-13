#include "Judg.h"
#include "../Scene/Preview.h"
#include "Player.h"
#include "Vita.h"
#include "Collider.h"
#include "GameVoice.h"

using namespace idea;

class Judg::Impl{
public:
	// プレイヤーは鬼か
	bool isPlayerOni_ = false;

	// プレイヤー逃走持続時間
	int playerTime_ = 0;

	// vita逃走持続時間
	int vitaTime_ = 0;

	// 勝者
	int winner_ = ABSENCE;

	Sound seTouch_;
};

Judg::Judg() :
	pImpl_(new Judg::Impl)
{
	Assert(pImpl_);
}

Judg::~Judg()
{
	SafeDelete(pImpl_);
}

void Judg::Init()
{
	// 実装
	auto& impl = (*pImpl_);

	impl.isPlayerOni_ = false;

	impl.playerTime_ = 0;
	impl.vitaTime_ = 0;

	impl.winner_ = ABSENCE;

	impl.seTouch_.LoadWaveFromStorage("asset/SOUND/batyuun100.wav");
}

void Judg::Update()
{
	// 実装
	auto& impl = (*pImpl_);

	// シーン
	auto& scene = (*pScene_);

	if(typeid(scene) == typeid(Preview)){
		return;
	}

	// プレイヤー
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// vita本体
	auto& vita = (*scene.GetGameObject<Vita>(VITA_LAYER));

	// 音声
	auto& voice = (*scene.GetGameObject<GameVoice>(GAME_VOICE_LAYER));

	// 勝敗が決まった時すべてのvitaを待機状態にする
	if(impl.winner_){
		auto vitas = scene.GetGameObjects<Vita>(VITA_LAYER);

		for(auto stopVita : vitas){
			stopVita->SetState(VitaState::Idol);
		}

		return;
	}

	// お互いのカウント中は時間を進めない
	if(player.GetState() == PlayerState::Count
		|| vita.GetState() == VitaState::Count){
		return;
	}

	// 逃走持続時間カウント
	if(impl.isPlayerOni_){
		++impl.vitaTime_;

		// vita勝利で決着
		if(impl.vitaTime_ >= TIME_UP){
			if(!impl.winner_){
				voice.VitaWin();
			}

			impl.winner_ = VITA_WIN;
		}
	} else{
		++impl.playerTime_;

		// プレイヤー勝利で決着
		if(impl.playerTime_ >= TIME_UP){
			if(!impl.winner_){
				voice.PlayerWin();
			}

			impl.winner_ = PLAYER_WIN;
		}
	}
}

int Judg::GetPlayerTime()
{
	return pImpl_->playerTime_;
}

int Judg::GetVitaTime()
{
	return pImpl_->vitaTime_;
}

int Judg::GetWinner()
{
	return pImpl_->winner_;
}

void Judg::SetPlayerOniFlag(bool flag)
{
	pImpl_->isPlayerOni_ = flag;

	pImpl_->seTouch_.Play();
}

bool Judg::IsPlayerOni()
{
	return pImpl_->isPlayerOni_;
}
