#include "Judg.h"
#include "../Scene/Preview.h"
#include "Player.h"
#include "Vita.h"
#include "Collider.h"
#include "GameVoice.h"

using namespace idea;

class Judg::Impl{
public:
	// �v���C���[�͋S��
	bool isPlayerOni_ = false;

	// �v���C���[������������
	int playerTime_ = 0;

	// vita������������
	int vitaTime_ = 0;

	// ����
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
	// ����
	auto& impl = (*pImpl_);

	impl.isPlayerOni_ = false;

	impl.playerTime_ = 0;
	impl.vitaTime_ = 0;

	impl.winner_ = ABSENCE;

	impl.seTouch_.LoadWaveFromStorage("asset/SOUND/batyuun100.wav");
}

void Judg::Update()
{
	// ����
	auto& impl = (*pImpl_);

	// �V�[��
	auto& scene = (*pScene_);

	if(typeid(scene) == typeid(Preview)){
		return;
	}

	// �v���C���[
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// vita�{��
	auto& vita = (*scene.GetGameObject<Vita>(VITA_LAYER));

	// ����
	auto& voice = (*scene.GetGameObject<GameVoice>(GAME_VOICE_LAYER));

	// ���s�����܂��������ׂĂ�vita��ҋ@��Ԃɂ���
	if(impl.winner_){
		auto vitas = scene.GetGameObjects<Vita>(VITA_LAYER);

		for(auto stopVita : vitas){
			stopVita->SetState(VitaState::Idol);
		}

		return;
	}

	// ���݂��̃J�E���g���͎��Ԃ�i�߂Ȃ�
	if(player.GetState() == PlayerState::Count
		|| vita.GetState() == VitaState::Count){
		return;
	}

	// �����������ԃJ�E���g
	if(impl.isPlayerOni_){
		++impl.vitaTime_;

		// vita�����Ō���
		if(impl.vitaTime_ >= TIME_UP){
			if(!impl.winner_){
				voice.VitaWin();
			}

			impl.winner_ = VITA_WIN;
		}
	} else{
		++impl.playerTime_;

		// �v���C���[�����Ō���
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
