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
	// プレイヤーのスキル評価値
	float playerSkillEvaluationValue_ = 0.0f;

	// 難易度
	float difficulty_ = 0.0f;

	// 難易度調整レート
	float correctionRate_ = 0.0f;

	// アイテム追加時間
	int addItemTime_ = 0;

	// アイテム追加間隔
	int itemSetInterval_ = 0;

	// vita追加時間
	int addVitaTime_ = 0;

	// 追加vita数
	int addVitaNum_ = 0;

	// vita追加間隔
	int vitaSetInterval_ = 0;

	// vitaの待機範囲
	float vitaIdolRange_ = 0.0f;

	// vitaの近づく速度
	float vitaStopBySpeed_ = 0.0f;

	// vitaの追いかける速度
	float vitaChaseSpeed_ = 0.0f;

	// vitaの逃げる速度
	float vitaEscapeSpeed_ = 0.0f;

	// vitaの坂を上る速度のレート
	float vitaClimbRate_ = 0.0f;

	// プレイヤーの逃走持続時間
	int playerSustain_ = 0;

	// プレイヤーの逃走持続時間の補助
	int playerSustainSub_ = 0;

	// vitaの逃走持続時間
	int vitaSustain_ = 0;

	// 前回のプレイヤーの逃走持続時間
	float prePlayerStamina_ = 0.0f;

	// オニが変わった瞬間
	bool bOniChangeFlag_ = false;

	// アイテムを取得した瞬間
	bool bGetItemFlag_ = false;

	// すぐに捕まった表示の時間
	int showGetCaughtImmediatelyCount_ = 0;

	// すぐに捕まえた表示の時間
	int showCatchImmediatelyCount_ = 0;

	// スタミナ切れ表示の時間
	int showOutOfStaminaCount_ = 0;

	// アイテム取得表示の時間
	int showGetItemCount_ = 0;

	// 乱数
	Random random_;

	// デバッグ情報
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
	// 実装
	auto& impl = (*pImpl_);

	// シーン
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

	// プレビュー時vitaの追いかける速度はちょっと早め
	if(typeid(scene) == typeid(Preview)){
		impl.vitaChaseSpeed_ = 0.045f;
	} else{
		impl.vitaChaseSpeed_ = 0.04f;
	}

	// プレビュー時vitaの逃げる速度はちょっと早め
	if(typeid(scene) == typeid(Preview)){
		impl.vitaEscapeSpeed_ = 0.055f;
	} else{
		impl.vitaEscapeSpeed_ = 0.04f;
	}

	// プレビュー時vitaの坂を上る速度はちょっと早め
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
	// 実装
	auto& impl = (*pImpl_);

	// シーン
	auto& scene = (*pScene_);

	// プレビュー時評価による調整は無し
	if(typeid(scene) == typeid(Preview)){
		return;
	}

	// プレイヤースキルの評価
	EvaluationPlayerSkill();

	// 難易度調整
	AdjustmentDifficulty();

	// AIレベル調整
	AdjustmentVitaLevel();

	// AIのセット
	SetVita();

	// アイテムのセット
	SetItem();

	impl.bOniChangeFlag_ = false;

	// デバッグ情報の表示
	ShowDebug();
}

void MetaAI::EvaluationPlayerSkill()
{
	// 実装
	auto& impl = (*pImpl_);

	// シーン
	auto& scene = (*pScene_);

	// プレイヤー
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// vita本体
	auto& vita = (*scene.GetGameObject<Vita>(VITA_LAYER));

	// 勝敗判定
	auto& judg = (*scene.GetGameObject<Judg>(JUDG_LAYER));

	// スタミナ切れによる減点
	if(player.GetExhausted() == 209){
		impl.playerSkillEvaluationValue_ -= 5.0f;

		impl.showOutOfStaminaCount_ = 180;
	}

	if(impl.showOutOfStaminaCount_){
		--impl.showOutOfStaminaCount_;

		impl.debugLog_.Log("スタミナ切れ　　：減点");
	}

	// アイテム獲得による加点
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

		impl.debugLog_.Log("アイテム獲得　　：加点");
	}

	// 接触タイミングによる評価
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
		impl.debugLog_.Log("すぐにつかまった：減点");
	}

	if(impl.showCatchImmediatelyCount_){
		--impl.showCatchImmediatelyCount_;
		impl.debugLog_.Log("すぐにつかまえた：加点");
	}

	// スタミナ使用頻度による評価
	if(player.GetStamina() < impl.prePlayerStamina_ || player.GetDoping()){
		impl.playerSkillEvaluationValue_ += 0.00075f;

		impl.prePlayerStamina_ = player.GetStamina();

		impl.debugLog_.Log("加速を使っている：加点");
	} else{
		impl.playerSkillEvaluationValue_ -= 0.00125f;

		impl.prePlayerStamina_ = player.GetStamina();

		impl.debugLog_.Log("加速を使ってない：減点");
	}

	// 時間経過による評価
	if(player.GetState() != PlayerState::Count && vita.GetState() != VitaState::Count){
		if(judg.IsPlayerOni()){
			if(judg.GetVitaTime() > judg.GetPlayerTime()){
				impl.playerSkillEvaluationValue_ -= 0.025f * (judg.GetVitaTime() / (float)TIME_UP);
			} else{
				impl.playerSkillEvaluationValue_ -= 0.02f * (judg.GetVitaTime() / (float)TIME_UP);
			}

			impl.debugLog_.Log("オニ継続　　　　：減点");
		} else{
			if(judg.GetPlayerTime() > judg.GetVitaTime()){
				impl.playerSkillEvaluationValue_ += 0.025f * (judg.GetPlayerTime() / (float)TIME_UP);
			} else{
				impl.playerSkillEvaluationValue_ += 0.02f * (judg.GetPlayerTime() / (float)TIME_UP);
			}

			impl.debugLog_.Log("逃走継続　　　　：加点");
		}
	}

	// 評価閾値
	impl.playerSkillEvaluationValue_ = Max(30.0f, impl.playerSkillEvaluationValue_);
	impl.playerSkillEvaluationValue_ = Min(100.0f, impl.playerSkillEvaluationValue_);

	impl.debugLog_.Log("評価要素-----------");
}

void MetaAI::AdjustmentDifficulty()
{
	// 実装
	auto& impl = (*pImpl_);

	// 難易度調整
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

	// 難易度閾値
	impl.difficulty_ = Max(20.0f, impl.difficulty_);
	impl.difficulty_ = Min(100.0f, impl.difficulty_);
}

void MetaAI::AdjustmentVitaLevel()
{
	// 実装
	auto& impl = (*pImpl_);

	// シーン
	auto& scene = (*pScene_);

	// vitaリスト
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
	// 実装
	auto& impl = (*pImpl_);

	// シーン
	auto& scene = (*pScene_);

	// vita本体
	auto& vita = (*scene.GetGameObject<Vita>(VITA_LAYER));

	// vitaリスト
	auto vitas = scene.GetGameObjects<Vita>(VITA_LAYER);

	// 勝敗判定
	auto& judg = (*scene.GetGameObject<Judg>(JUDG_LAYER));

	// 音声
	auto& voice = (*scene.GetGameObject<GameVoice>(GAME_VOICE_LAYER));

	// AI追加上限
	if(vitas.size() >= 10){
		impl.addVitaTime_ = 0;
	}

	// AI追加
	if(judg.IsPlayerOni()){
		// AIが捕まった時に一体のみ残し消去
		if(impl.bOniChangeFlag_){
			for(auto& vita : vitas){
				if(vita->GetRole() != VitaRole::Main){
					vita->Disappearance();
				} else{
					vita->Idol();
				}
			}

			// 追加時間初期化
			impl.addVitaTime_ = 0;
			impl.addVitaNum_ = 0;
		}
	} else{
		// 時間経過による追加
		if(vita.GetState() != VitaState::Count){
			++impl.addVitaTime_;
		}

		if(impl.addVitaTime_ > impl.vitaSetInterval_){
			AddVita();

			// 追加時間初期化
			impl.addVitaTime_ = 0;
			impl.addVitaNum_ = 0;

			voice.AddVita(vitas.size());
		}

		// AI追跡ターンに切り替わった時の補填
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
	// 実装
	auto& impl = (*pImpl_);

	// シーン
	auto& scene = (*pScene_);

	// プレイヤー
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// マップ情報
	auto& map = (*scene.GetGameObject<Map>(MAP_LAYER));

	// 勝敗判定
	auto& judg = (*scene.GetGameObject<Judg>(JUDG_LAYER));

	// 行
	int col = map.GetCol();

	// 列
	int row = map.GetRow();

	// vitaリスト
	auto vitas = scene.GetGameObjects<Vita>(VITA_LAYER);

	// vita追加セルX座標
	int cellX = impl.random_.GetRand(0, col - 1);

	// vita追加セルZ座標
	int cellZ = impl.random_.GetRand(0, row - 1);

	// 位置が被らなくなるまで再抽選
	for(int i = 0; i < 1000; ++i){
		// 位置が被っていないか
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

	// vita追加X座標
	float x = CenterAlign(0.0f, 10.0f, cellX, col);

	// vita追加Z座標
	float z = CenterAlign(0.0f, 10.0f, (row - 1) - cellZ, row);

	// 追加vita
	auto& newVita = (*scene.AddGameObject<Vita>(VITA_LAYER));

	newVita.SetPosition(x, 10.0f, z);

	newVita.Appearance();

	// vita総数によって役割を変更
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

	// vita総数によって速度を調整
	newVita.SetChaseSpeedRate(1.0f - 0.02f * vitas.size());

	if(impl.addVitaNum_){
		--impl.addVitaNum_;
	}
}

void MetaAI::SetItem()
{
	// 実装
	auto& impl = (*pImpl_);

	// シーン
	auto& scene = (*pScene_);

	// プレイヤー
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// マップ情報
	auto& map = (*scene.GetGameObject<Map>(MAP_LAYER));

	// アイテムリスト
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
			// 行
			int col = map.GetCol();

			// 列
			int row = map.GetRow();

			// アイテム追加セルX座標
			int cellX = impl.random_.GetRand(0, col - 1);

			// アイテム追加セルZ座標
			int cellZ = impl.random_.GetRand(0, row - 1);

			// 位置が被らなくなるまで再抽選
			for(int i = 0; i < 1000; ++i){
				// 位置が被っていないか
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

			// アイテム追加X座標
			float x = CenterAlign(0.0f, 10.0f, cellX, col);

			// アイテム追加Z座標
			float z = CenterAlign(0.0f, 10.0f, (row - 1) - cellZ, row);

			// アイテム追加
			scene.AddGameObject<Item>(ITEM_LAYER)->SetPosition(x, 10.0f, z);

			impl.addItemTime_ = 0;
		}
	} else{
		impl.addItemTime_ = 0;
	}
}

void MetaAI::ShowDebug()
{
	// 実装
	auto& impl = (*pImpl_);

	// シーン
	auto& scene = (*pScene_);

	// vitaリスト
	auto vitas = scene.GetGameObjects<Vita>(VITA_LAYER);

	// 勝敗判定
	auto& judg = (*scene.GetGameObject<Judg>(JUDG_LAYER));

	// 追いかけるvitaの数
	int chaseNum = 0;

	// 先回りするvitaの数
	int advanceNum = 0;

	// 待ち伏せするvitaの数
	int ambushNum = 0;

	if(!judg.IsPlayerOni()){
		for(auto& vita : vitas){
			switch(vita->GetRole()){
				// 本体
				case VitaRole::Main:
					++chaseNum;
					break;

				// 追いかける
				case VitaRole::Chase:
					++chaseNum;
					break;

				// 先回り
				case VitaRole::Wrap:
					++advanceNum;
					break;

				// 待ち伏せ
				case VitaRole::Ambush:
					++ambushNum;
					break;
			}
		}
	}

	impl.debugLog_.Log("C追尾:%d W先回り:%d A待ち伏せ:%d", chaseNum, advanceNum, ambushNum);
	impl.debugLog_.Log("AI人数　　　　　：%d", (int)(vitas.size()));
	impl.debugLog_.Log("AI追加間隔　　　：%d", impl.vitaSetInterval_);
	impl.debugLog_.Log("アイテム排出間隔：%d", impl.itemSetInterval_);
	impl.debugLog_.Log("AI逃走速度　　　：%0.2f", (float)(impl.vitaEscapeSpeed_ * 100.0f));
	impl.debugLog_.Log("AI追跡速度　　　：%0.2f", (float)(impl.vitaChaseSpeed_ * 100.0f));
	impl.debugLog_.Log("AI坂登速度　　　：%0.2f", 10.0f - (float)(impl.vitaClimbRate_ * 10.0f));
	impl.debugLog_.Log("難易度内訳---------");
	impl.debugLog_.Log("難易度調整レート：%0.2f", (float)(impl.correctionRate_ * 1000.0f));
	impl.debugLog_.Log("難易度　　　　　：%0.1f", impl.difficulty_);
	impl.debugLog_.Log("プレイヤースキル：%0.1f", impl.playerSkillEvaluationValue_);
	if(impl.difficulty_ >= impl.playerSkillEvaluationValue_ + 3.0f){
		impl.debugLog_.Log("プレイヤー心理　：緊張");
	} else if(impl.difficulty_ <= impl.playerSkillEvaluationValue_ - 3.0f){
		impl.debugLog_.Log("プレイヤー心理　：リラックス");
	} else{
		impl.debugLog_.Log("プレイヤー心理　：フロー");
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

