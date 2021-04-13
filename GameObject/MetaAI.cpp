#include "MetaAI.h"
#include "../Scene/Preview.h"
#include "Player.h"
#include "Vita.h"
#include "Item.h"
#include "Map.h"
#include "Judg.h"
#include "GameVoice.h"

using namespace idea;

class MetaAI::Impl{
public:
	// �v���C���[�̃X�L���]���l
	float playerSkillEvaluationValue_ = 0.0f;

	// ��Փx
	float difficulty_ = 0.0f;

	// ��Փx�������[�g
	float correctionRate_ = 0.0f;

	// �A�C�e���ǉ�����
	int addItemTime_ = 0;

	// �A�C�e���ǉ��Ԋu
	int itemSetInterval_ = 0;

	// vita�ǉ�����
	int addVitaTime_ = 0;

	// �ǉ�vita��
	int addVitaNum_ = 0;

	// vita�ǉ��Ԋu
	int vitaSetInterval_ = 0;

	// vita�̑ҋ@�͈�
	float vitaIdolRange_ = 0.0f;

	// vita�̋߂Â����x
	float vitaStopBySpeed_ = 0.0f;

	// vita�̒ǂ������鑬�x
	float vitaChaseSpeed_ = 0.0f;

	// vita�̓����鑬�x
	float vitaEscapeSpeed_ = 0.0f;

	// vita�̍����鑬�x�̃��[�g
	float vitaClimbRate_ = 0.0f;

	// �v���C���[�̓�����������
	int playerSustain_ = 0;

	// �v���C���[�̓����������Ԃ̕⏕
	int playerSustainSub_ = 0;

	// vita�̓�����������
	int vitaSustain_ = 0;

	// �O��̃v���C���[�̓�����������
	float prePlayerStamina_ = 0.0f;

	// �I�j���ς�����u��
	bool bOniChangeFlag_ = false;

	// �A�C�e�����擾�����u��
	bool bGetItemFlag_ = false;

	// �����ɕ߂܂����\���̎���
	int showGetCaughtImmediatelyCount_ = 0;

	// �����ɕ߂܂����\���̎���
	int showCatchImmediatelyCount_ = 0;

	// �X�^�~�i�؂�\���̎���
	int showOutOfStaminaCount_ = 0;

	// �A�C�e���擾�\���̎���
	int showGetItemCount_ = 0;

	// ����
	Random random_;

	// �f�o�b�O���
	Debug debugLog_;
};

MetaAI::MetaAI() :
	pImpl_(new MetaAI::Impl)
{
	Assert(pImpl_);
}

MetaAI::~MetaAI()
{
	SafeDelete(pImpl_);
}

void MetaAI::Init()
{
	// ����
	auto& impl = (*pImpl_);

	// �V�[��
	auto& scene = (*pScene_);

	impl.playerSkillEvaluationValue_ = 50.0f;

	impl.difficulty_ = 50.0f;

	impl.correctionRate_ = 0.0005f;

	impl.addItemTime_ = 0;

	impl.addVitaTime_ = 0;

	impl.addVitaNum_ = 0;

	impl.vitaSetInterval_ = 10 * 60;

	impl.itemSetInterval_ = 3 * 60;

	impl.vitaIdolRange_ = 40.0f;

	impl.vitaStopBySpeed_ = 0.04f;

	// �v���r���[��vita�̒ǂ������鑬�x�͂�����Ƒ���
	if(typeid(scene) == typeid(Preview)){
		impl.vitaChaseSpeed_ = 0.045f;
	} else{
		impl.vitaChaseSpeed_ = 0.04f;
	}

	// �v���r���[��vita�̓����鑬�x�͂�����Ƒ���
	if(typeid(scene) == typeid(Preview)){
		impl.vitaEscapeSpeed_ = 0.055f;
	} else{
		impl.vitaEscapeSpeed_ = 0.04f;
	}

	// �v���r���[��vita�̍����鑬�x�͂�����Ƒ���
	if(typeid(scene) == typeid(Preview)){
		impl.vitaClimbRate_ = 0.2f;
	} else{
		impl.vitaClimbRate_ = 0.7f;
	}

	impl.playerSustain_ = 0;

	impl.playerSustainSub_ = 0;

	impl.vitaSustain_ = 0;

	impl.prePlayerStamina_ = 300.0f;

	impl.bOniChangeFlag_ = false;

	impl.bGetItemFlag_ = false;

	impl.showGetCaughtImmediatelyCount_ = 0;

	impl.showCatchImmediatelyCount_ = 0;

	impl.showOutOfStaminaCount_ = 0;

	impl.showGetItemCount_ = 0;
}

void MetaAI::Update()
{
	// ����
	auto& impl = (*pImpl_);

	// �V�[��
	auto& scene = (*pScene_);

	// �v���r���[���]���ɂ�钲���͖���
	if(typeid(scene) == typeid(Preview)){
		return;
	}

	// �v���C���[�X�L���̕]��
	EvaluationPlayerSkill();

	// ��Փx����
	AdjustmentDifficulty();

	// AI���x������
	AdjustmentVitaLevel();

	// AI�̃Z�b�g
	SetVita();

	// �A�C�e���̃Z�b�g
	SetItem();

	impl.bOniChangeFlag_ = false;

	// �f�o�b�O���̕\��
	ShowDebug();
}

void MetaAI::EvaluationPlayerSkill()
{
	// ����
	auto& impl = (*pImpl_);

	// �V�[��
	auto& scene = (*pScene_);

	// �v���C���[
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// vita�{��
	auto& vita = (*scene.GetGameObject<Vita>(VITA_LAYER));

	// ���s����
	auto& judg = (*scene.GetGameObject<Judg>(JUDG_LAYER));

	// �X�^�~�i�؂�ɂ�錸�_
	if(player.GetExhausted() == 209){
		impl.playerSkillEvaluationValue_ -= 5.0f;

		impl.showOutOfStaminaCount_ = 180;
	}

	if(impl.showOutOfStaminaCount_){
		--impl.showOutOfStaminaCount_;

		impl.debugLog_.Log("�X�^�~�i�؂�@�@�F���_");
	}

	// �A�C�e���l���ɂ����_
	if(impl.bGetItemFlag_){
		if(judg.GetVitaTime() > judg.GetPlayerTime()){
			impl.playerSkillEvaluationValue_ += 1.0f;
		} else{
			impl.playerSkillEvaluationValue_ += 3.0f;
		}

		impl.bGetItemFlag_ = false;

		impl.showGetItemCount_ = 180;
	}

	if(impl.showGetItemCount_){
		--impl.showGetItemCount_;

		impl.debugLog_.Log("�A�C�e���l���@�@�F���_");
	}

	// �ڐG�^�C�~���O�ɂ��]��
	if(judg.IsPlayerOni()){
		if(impl.bOniChangeFlag_){
			if(impl.playerSustain_ < 10 * 60){
				if(judg.GetVitaTime() > judg.GetPlayerTime()){
					impl.playerSkillEvaluationValue_ -= (10 * 60 - impl.playerSustain_) * 0.02f;
				} else{
					impl.playerSkillEvaluationValue_ -= (10 * 60 - impl.playerSustain_) * 0.01f;
				}

				impl.showGetCaughtImmediatelyCount_ = 180;
			}

			impl.playerSustain_ = 0;
		}

		++impl.vitaSustain_;
	} else{
		if(impl.bOniChangeFlag_){
			if(impl.vitaSustain_ < 10 * 60){
				if(judg.GetVitaTime() > judg.GetPlayerTime()){
					impl.playerSkillEvaluationValue_ += (10 * 60 - impl.vitaSustain_) * 0.01f;
				} else{
					impl.playerSkillEvaluationValue_ += (10 * 60 - impl.vitaSustain_) * 0.02f;
				}

				impl.showCatchImmediatelyCount_ = 180;
			}

			impl.vitaSustain_ = 0;
		}

		++impl.playerSustain_;
	}

	if(impl.showGetCaughtImmediatelyCount_){
		--impl.showGetCaughtImmediatelyCount_;
		impl.debugLog_.Log("�����ɂ��܂����F���_");
	}

	if(impl.showCatchImmediatelyCount_){
		--impl.showCatchImmediatelyCount_;
		impl.debugLog_.Log("�����ɂ��܂����F���_");
	}

	// �X�^�~�i�g�p�p�x�ɂ��]��
	if(player.GetStamina() < impl.prePlayerStamina_ || player.GetDoping()){
		impl.playerSkillEvaluationValue_ += 0.00075f;

		impl.prePlayerStamina_ = player.GetStamina();

		impl.debugLog_.Log("�������g���Ă���F���_");
	} else{
		impl.playerSkillEvaluationValue_ -= 0.00125f;

		impl.prePlayerStamina_ = player.GetStamina();

		impl.debugLog_.Log("�������g���ĂȂ��F���_");
	}

	// ���Ԍo�߂ɂ��]��
	if(player.GetState() != PlayerState::Count && vita.GetState() != VitaState::Count){
		if(judg.IsPlayerOni()){
			if(judg.GetVitaTime() > judg.GetPlayerTime()){
				impl.playerSkillEvaluationValue_ -= 0.025f * (judg.GetVitaTime() / (float)TIME_UP);
			} else{
				impl.playerSkillEvaluationValue_ -= 0.02f * (judg.GetVitaTime() / (float)TIME_UP);
			}

			impl.debugLog_.Log("�I�j�p���@�@�@�@�F���_");
		} else{
			if(judg.GetPlayerTime() > judg.GetVitaTime()){
				impl.playerSkillEvaluationValue_ += 0.025f * (judg.GetPlayerTime() / (float)TIME_UP);
			} else{
				impl.playerSkillEvaluationValue_ += 0.02f * (judg.GetPlayerTime() / (float)TIME_UP);
			}

			impl.debugLog_.Log("�����p���@�@�@�@�F���_");
		}
	}

	// �]��臒l
	impl.playerSkillEvaluationValue_ = Max(30.0f, impl.playerSkillEvaluationValue_);
	impl.playerSkillEvaluationValue_ = Min(100.0f, impl.playerSkillEvaluationValue_);

	impl.debugLog_.Log("�]���v�f-----------");
}

void MetaAI::AdjustmentDifficulty()
{
	// ����
	auto& impl = (*pImpl_);

	// ��Փx����
	impl.correctionRate_ = 0.0005f;

	if(Abs(impl.difficulty_ - impl.playerSkillEvaluationValue_) >= 3.0f){
		impl.correctionRate_ += 0.001f;
	}

	if(Abs(impl.difficulty_ - impl.playerSkillEvaluationValue_) >= 5.0f){
		impl.correctionRate_ += 0.001f;
	}

	if(Abs(impl.difficulty_ - impl.playerSkillEvaluationValue_) >= 7.0f){
		impl.correctionRate_ += 0.001f;
	}

	if(Abs(impl.difficulty_ > impl.playerSkillEvaluationValue_)){
		impl.correctionRate_ += 0.001f;
	}

	impl.difficulty_ += (impl.playerSkillEvaluationValue_ - impl.difficulty_) * impl.correctionRate_;

	// ��Փx臒l
	impl.difficulty_ = Max(20.0f, impl.difficulty_);
	impl.difficulty_ = Min(100.0f, impl.difficulty_);
}

void MetaAI::AdjustmentVitaLevel()
{
	// ����
	auto& impl = (*pImpl_);

	// �V�[��
	auto& scene = (*pScene_);

	// vita���X�g
	auto vitas = scene.GetGameObjects<Vita>(VITA_LAYER);

	impl.vitaChaseSpeed_ = 0.045f + 0.0275f * (impl.difficulty_ - 50.0f) / 50.0f;
	impl.vitaEscapeSpeed_ = 0.045f + 0.0375f * (impl.difficulty_ - 50.0f) / 50.0f;

	impl.vitaChaseSpeed_ = Max(0.04f, impl.vitaChaseSpeed_);
	impl.vitaEscapeSpeed_ = Max(0.04f, impl.vitaEscapeSpeed_);

	impl.vitaChaseSpeed_ = Min(0.065f, impl.vitaChaseSpeed_);
	impl.vitaEscapeSpeed_ = Min(0.07f, impl.vitaEscapeSpeed_);

	impl.vitaStopBySpeed_ = Max(impl.vitaStopBySpeed_, impl.vitaChaseSpeed_) + 0.00000125f;

	if(impl.difficulty_ < 50.0f){
		impl.vitaIdolRange_ = 30.0f;
	} else if(impl.difficulty_ < 65.0f){
		impl.vitaIdolRange_ = 35.0f;
	} else if(impl.difficulty_ < 75.0f){
		impl.vitaIdolRange_ = 40.0f;
	} else{
		impl.vitaIdolRange_ = 50.0f;
	}

	if(impl.difficulty_ < 50.0f){
		impl.vitaClimbRate_ = 0.7f;
	} else if(impl.difficulty_ < 65.0f){
		impl.vitaClimbRate_ = 0.675f;
	} else if(impl.difficulty_ < 75.0f){
		impl.vitaClimbRate_ = 0.65f;
	} else{
		impl.vitaClimbRate_ = 0.6f;
	}

	if(impl.difficulty_ < 50.0f){
		impl.vitaSetInterval_ = 10 * 60;
	} else if(impl.difficulty_ < 65.0f){
		impl.vitaSetInterval_ = 8 * 60;
	} else if(impl.difficulty_ < 75.0f){
		impl.vitaSetInterval_ = 6 * 60;
	} else{
		impl.vitaSetInterval_ = 4 * 60;
	}
}

void MetaAI::SetVita()
{
	// ����
	auto& impl = (*pImpl_);

	// �V�[��
	auto& scene = (*pScene_);

	// vita�{��
	auto& vita = (*scene.GetGameObject<Vita>(VITA_LAYER));

	// vita���X�g
	auto vitas = scene.GetGameObjects<Vita>(VITA_LAYER);

	// ���s����
	auto& judg = (*scene.GetGameObject<Judg>(JUDG_LAYER));

	// ����
	auto& voice = (*scene.GetGameObject<GameVoice>(GAME_VOICE_LAYER));

	// AI�ǉ����
	if(vitas.size() >= 10){
		impl.addVitaTime_ = 0;
	}

	// AI�ǉ�
	if(judg.IsPlayerOni()){
		// AI���߂܂������Ɉ�̂̂ݎc������
		if(impl.bOniChangeFlag_){
			for(auto& vita : vitas){
				if(vita->GetRole() != VitaRole::Main){
					vita->Disappearance();
				} else{
					vita->Idol();
				}
			}

			// �ǉ����ԏ�����
			impl.addVitaTime_ = 0;
			impl.addVitaNum_ = 0;
		}
	} else{
		// ���Ԍo�߂ɂ��ǉ�
		if(vita.GetState() != VitaState::Count){
			++impl.addVitaTime_;
		}

		if(impl.addVitaTime_ > impl.vitaSetInterval_){
			AddVita();

			// �ǉ����ԏ�����
			impl.addVitaTime_ = 0;
			impl.addVitaNum_ = 0;

			voice.AddVita(vitas.size());
		}

		// AI�ǐՃ^�[���ɐ؂�ւ�������̕�U
		if(impl.bOniChangeFlag_){
			if(impl.difficulty_ < 65.0f){
				impl.addVitaNum_ = 0;
			} else if(impl.difficulty_ < 75.0f){
				impl.addVitaNum_ = 3;
				voice.FlockVita();
			} else if(impl.difficulty_ < 85.0f){
				impl.addVitaNum_ = 5;
				voice.FlockVita();
			} else{
				impl.addVitaNum_ = 7;
				voice.FlockVita();
			}

			impl.addVitaTime_ = 0;
		}

		if(impl.addVitaNum_ > 0){
			AddVita();
		}
	}
}

void MetaAI::AddVita()
{
	// ����
	auto& impl = (*pImpl_);

	// �V�[��
	auto& scene = (*pScene_);

	// �v���C���[
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// �}�b�v���
	auto& map = (*scene.GetGameObject<Map>(MAP_LAYER));

	// ���s����
	auto& judg = (*scene.GetGameObject<Judg>(JUDG_LAYER));

	// �s
	int col = map.GetCol();

	// ��
	int row = map.GetRow();

	// vita���X�g
	auto vitas = scene.GetGameObjects<Vita>(VITA_LAYER);

	// vita�ǉ��Z��X���W
	int cellX = impl.random_.GetRand(0, col - 1);

	// vita�ǉ��Z��Z���W
	int cellZ = impl.random_.GetRand(0, row - 1);

	// �ʒu�����Ȃ��Ȃ�܂ōĒ��I
	for(int i = 0; i < 1000; ++i){
		// �ʒu������Ă��Ȃ���
		bool check = true;

		if(player.GetCell() == Cell(cellX, cellZ)){
			check = false;
		}

		for(auto vita : vitas){
			if(vita->GetCell() == Cell(cellX, cellZ)){
				check = false;
			}
		}

		if(check){
			break;
		}

		cellX = impl.random_.GetRand(0, col - 1);
		cellZ = impl.random_.GetRand(0, row - 1);
	}

	// vita�ǉ�X���W
	float x = CenterAlign(0.0f, 10.0f, cellX, col);

	// vita�ǉ�Z���W
	float z = CenterAlign(0.0f, 10.0f, (row - 1) - cellZ, row);

	// �ǉ�vita
	auto& newVita = (*scene.AddGameObject<Vita>(VITA_LAYER));

	newVita.SetPosition(x, 10.0f, z);

	newVita.Appearance();

	// vita�����ɂ���Ė�����ύX
	switch((vitas.size() - 1) % 3){
		case 0:
			if(judg.GetPlayerTime() < 40 * 60){
				newVita.SetRole(VitaRole::Ambush);
			} else{
				newVita.SetRole(VitaRole::Wrap);
			}
			break;

		case 1:
			newVita.SetRole(VitaRole::Chase);
			break;

		case 2:
			newVita.SetRole(VitaRole::Wrap);
			break;
	}

	// vita�����ɂ���đ��x�𒲐�
	newVita.SetChaseSpeedRate(1.0f - 0.02f * vitas.size());

	if(impl.addVitaNum_){
		--impl.addVitaNum_;
	}
}

void MetaAI::SetItem()
{
	// ����
	auto& impl = (*pImpl_);

	// �V�[��
	auto& scene = (*pScene_);

	// �v���C���[
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// �}�b�v���
	auto& map = (*scene.GetGameObject<Map>(MAP_LAYER));

	// �A�C�e�����X�g
	auto items = scene.GetGameObjects<Item>(ITEM_LAYER);

	++impl.addItemTime_;

	if(impl.difficulty_ < 50.0f){
		impl.itemSetInterval_ = 5 * 60;
	} else if(impl.difficulty_ < 60.0f){
		impl.itemSetInterval_ = 8 * 60;
	} else{
		impl.itemSetInterval_ = 10 * 60;
	}

	if(items.size() < 4){
		if(impl.addItemTime_ > impl.itemSetInterval_){
			// �s
			int col = map.GetCol();

			// ��
			int row = map.GetRow();

			// �A�C�e���ǉ��Z��X���W
			int cellX = impl.random_.GetRand(0, col - 1);

			// �A�C�e���ǉ��Z��Z���W
			int cellZ = impl.random_.GetRand(0, row - 1);

			// �ʒu�����Ȃ��Ȃ�܂ōĒ��I
			for(int i = 0; i < 1000; ++i){
				// �ʒu������Ă��Ȃ���
				bool check = true;

				if(player.GetCell() == Cell(cellX, cellZ)){
					check = false;
				}

				for(auto item : items){
					if(item->GetCell() == Cell(cellX, cellZ)){
						check = false;
					}
				}

				if(check){
					break;
				}

				cellX = impl.random_.GetRand(0, col - 1);
				cellZ = impl.random_.GetRand(0, row - 1);
			}

			// �A�C�e���ǉ�X���W
			float x = CenterAlign(0.0f, 10.0f, cellX, col);

			// �A�C�e���ǉ�Z���W
			float z = CenterAlign(0.0f, 10.0f, (row - 1) - cellZ, row);

			// �A�C�e���ǉ�
			scene.AddGameObject<Item>(ITEM_LAYER)->SetPosition(x, 10.0f, z);

			impl.addItemTime_ = 0;
		}
	} else{
		impl.addItemTime_ = 0;
	}
}

void MetaAI::ShowDebug()
{
	// ����
	auto& impl = (*pImpl_);

	// �V�[��
	auto& scene = (*pScene_);

	// vita���X�g
	auto vitas = scene.GetGameObjects<Vita>(VITA_LAYER);

	// ���s����
	auto& judg = (*scene.GetGameObject<Judg>(JUDG_LAYER));

	// �ǂ�������vita�̐�
	int chaseNum = 0;

	// ���肷��vita�̐�
	int advanceNum = 0;

	// �҂���������vita�̐�
	int ambushNum = 0;

	if(!judg.IsPlayerOni()){
		for(auto& vita : vitas){
			switch(vita->GetRole()){
				// �{��
				case VitaRole::Main:
					++chaseNum;
					break;

				// �ǂ�������
				case VitaRole::Chase:
					++chaseNum;
					break;

				// ����
				case VitaRole::Wrap:
					++advanceNum;
					break;

				// �҂�����
				case VitaRole::Ambush:
					++ambushNum;
					break;
			}
		}
	}

	impl.debugLog_.Log("C�ǔ�:%d W����:%d A�҂�����:%d", chaseNum, advanceNum, ambushNum);
	impl.debugLog_.Log("AI�l���@�@�@�@�@�F%d", (int)(vitas.size()));
	impl.debugLog_.Log("AI�ǉ��Ԋu�@�@�@�F%d", impl.vitaSetInterval_);
	impl.debugLog_.Log("�A�C�e���r�o�Ԋu�F%d", impl.itemSetInterval_);
	impl.debugLog_.Log("AI�������x�@�@�@�F%0.2f", (float)(impl.vitaEscapeSpeed_ * 100.0f));
	impl.debugLog_.Log("AI�ǐՑ��x�@�@�@�F%0.2f", (float)(impl.vitaChaseSpeed_ * 100.0f));
	impl.debugLog_.Log("AI��o���x�@�@�@�F%0.2f", 10.0f - (float)(impl.vitaClimbRate_ * 10.0f));
	impl.debugLog_.Log("��Փx����---------");
	impl.debugLog_.Log("��Փx�������[�g�F%0.2f", (float)(impl.correctionRate_ * 1000.0f));
	impl.debugLog_.Log("��Փx�@�@�@�@�@�F%0.1f", impl.difficulty_);
	impl.debugLog_.Log("�v���C���[�X�L���F%0.1f", impl.playerSkillEvaluationValue_);
	if(impl.difficulty_ >= impl.playerSkillEvaluationValue_ + 3.0f){
		impl.debugLog_.Log("�v���C���[�S���@�F�ْ�");
	} else if(impl.difficulty_ <= impl.playerSkillEvaluationValue_ - 3.0f){
		impl.debugLog_.Log("�v���C���[�S���@�F�����b�N�X");
	} else{
		impl.debugLog_.Log("�v���C���[�S���@�F�t���[");
	}
}

float MetaAI::GetVitaIdolRange()
{
	return pImpl_->vitaIdolRange_;
}

float MetaAI::GetVitaStopBySpeed()
{
	return pImpl_->vitaStopBySpeed_;
}

float MetaAI::GetVitaChaseSpeed()
{
	return pImpl_->vitaChaseSpeed_;
}

float MetaAI::GetVitaEscapeSpeed()
{
	return pImpl_->vitaEscapeSpeed_;
}

float MetaAI::GetVitaClimbRate()
{
	return pImpl_->vitaClimbRate_;
}

void MetaAI::OniChange()
{
	pImpl_->bOniChangeFlag_ = true;
}

void MetaAI::GetItem()
{
	pImpl_->bGetItemFlag_ = true;
}

