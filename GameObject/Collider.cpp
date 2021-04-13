#include "Collider.h"
#include "../Scene/Preview.h"
#include "Player.h"
#include "vita.h"
#include "Block.h"
#include "Item.h"
#include "Judg.h"
#include "MetaAI.h"
#include "TouchEffect.h"
#include "GameVoice.h"

using namespace idea;

void Collider::Update()
{
	// シーン
	auto& scene = (*pScene_);

	// プレイヤー
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// vitaリスト
	auto vitas = scene.GetGameObjects<Vita>(VITA_LAYER);

	// ブロックリスト
	auto blocks = scene.GetGameObjects<Block>(BLOCK_LAYER);

	// アイテムリスト
	auto items = scene.GetGameObjects<Item>(ITEM_LAYER);

	// 勝敗判定
	auto& judg = (*scene.GetGameObject<Judg>(JUDG_LAYER));

	// メタAI
	auto& metaAI = (*scene.GetGameObject<MetaAI>(META_AI_LAYER));

	// 音声
	auto& voice = (*scene.GetGameObject<GameVoice>(GAME_VOICE_LAYER));

	// プレイヤーとAIの衝突判定
	for(auto& vita : vitas){
		// 消滅時は衝突判定なし
		if(vita->GetState() == VitaState::Disappearance){
			continue;
		}

		if(typeid(scene) != typeid(Preview)){
			// ゲームモード
			if(vita->GetCollider().CheckHit(player.GetCollider())){
				// vitaの座標	
				Vector3D vitaPosition = vita->GetPosition();

				// ノックバックベクトル
				Vector3D backVector = vita->GetPosition() - player.GetPosition();

				// 押し出した先の座標
				Vector3D vitaEscapePosition = vita->GetCollider().CollisionEscape(player.GetCollider(), 0.0f);

				// vitaの衝突リアクション
				if(vita->GetState() != VitaState::Appearance
					&& vita->GetState() != VitaState::Count){
					vita->HitObstacle(vitaEscapePosition, backVector.ReplaceY(0).Normalized() * 0.4f);
				}

				if(judg.IsPlayerOni()){
					// プレイヤー　追跡->逃走
					if(player.GetState() != PlayerState::Count){
						judg.SetPlayerOniFlag(false);

						metaAI.OniChange();

						vita->Count();

						// エフェクト生成
						auto& effect = (*scene.AddGameObject<TouchEffect>(TOUCH_EFFECT_LAYER));

						// エフェクト発生座標
						Vector3D touchPosition = vita->GetPosition();
						touchPosition.y += 5.0f;

						effect.SetPosition(touchPosition);
						effect.SetColor(ideaColor::SOFT_RED);

						voice.PlayerTouch();
					}
				} else{
					// プレイヤー　逃走->追跡
					// vitaの座標	
					Vector3D playerPosition = player.GetPosition();

					// ノックバックベクトル
					Vector3D backVector = player.GetPosition() - vita->GetPosition();

					// 押し出した先の座標
					Vector3D playerEscapePosition = player.GetCollider().CollisionEscape(vita->GetCollider(), 0.0f);

					// vitaの衝突リアクション
					player.HitObstacle(playerEscapePosition, backVector.ReplaceY(0).Normalized() * 0.4f);

					if(!(vita->GetState() == VitaState::Count
						|| vita->GetState() == VitaState::Appearance)){
						judg.SetPlayerOniFlag(true);

						metaAI.OniChange();

						// タッチしたAI以外は消滅
						for(auto& vita2 : vitas){
							if(vita2 == vita){
								continue;
							}

							vita2->SetRole(VitaRole::Chase);
							vita2->Disappearance();
						}

						vita->SetRole(VitaRole::Main);

						vita->Idol();

						player.Count();

						// エフェクト生成
						auto& effect = (*scene.AddGameObject<TouchEffect>(TOUCH_EFFECT_LAYER));

						// エフェクト発生座標
						Vector3D touchPosition = player.GetPosition();
						touchPosition.y += 5.0f;

						effect.SetPosition(touchPosition);
						effect.SetColor(ideaColor::SOFT_BLUE);

						voice.VitaTouch();

						break;
					}
				}
			}
		} else{
			// プレビューモード
			if(vita->GetCollider().CheckHit(player.GetCollider())){
				// vita
				Vector3D vitaEscapePosition = vita->GetCollider().CollisionEscape(player.GetCollider(), 0.0f);
				vita->HitObstacle(vitaEscapePosition, Vector3DZero());

				// プレイヤー
				Vector3D playerEscapePosition = player.GetCollider().CollisionEscape(vita->GetCollider(), 0.0f);
				player.HitObstacle(playerEscapePosition, Vector3DZero());
			}
		}
	}

	// AI同士の衝突
	for(auto& vita : vitas){
		// 消滅時は衝突判定なし
		if(vita->GetState() == VitaState::Disappearance){
			continue;
		}

		for(auto& vita2 : vitas){
			// 消滅時は衝突判定なし
			if(vita2->GetState() == VitaState::Disappearance){
				continue;
			}

			// 自分とは衝突判定なし
			if(vita == vita2){
				continue;
			}

			if(vita->GetCollider().CheckHit(vita2->GetCollider())){
				if(typeid(scene) != typeid(Preview)){
					// ゲームモード
					{
						Vector3D backVector = vita2->GetPosition() - vita->GetPosition();
						Vector3D vitaEscapePosition = vita->GetCollider().CollisionEscape(vita2->GetCollider(), 0.0f);
						vita->HitObstacle(vitaEscapePosition, backVector.ReplaceY(0).Normalized() * 0.5f);
					}
					{
						Vector3D backVector = vita->GetPosition() - vita2->GetPosition();
						Vector3D vitaEscapePosition = vita2->GetCollider().CollisionEscape(vita->GetCollider(), 0.0f);
						vita->HitObstacle(vitaEscapePosition, backVector.ReplaceY(0).Normalized() * 0.5f);
					}
				} else{
					// プレビューモード
					{
						Vector3D backVector = vita2->GetPosition() - vita->GetPosition();
						Vector3D vitaEscapePosition = vita->GetCollider().CollisionEscape(vita2->GetCollider(), 0.0f);
						vita->HitObstacle(vitaEscapePosition, backVector.ReplaceY(0).Normalized() * 0.01f);
					}
					{
						Vector3D backVector = vita->GetPosition() - vita2->GetPosition();
						Vector3D vitaEscapePosition = vita2->GetCollider().CollisionEscape(vita->GetCollider(), 0.0f);
						vita->HitObstacle(vitaEscapePosition, backVector.ReplaceY(0).Normalized() * 0.01f);
					}
				}
			}
		}
	}

	// アイテムの取得
	for(auto& item : items){
		if(player.GetCollider().CheckHit(item->GetCollider())){
			item->SetDestroy();

			player.Doping();

			metaAI.GetItem();

			break;
		}
	}

	// 地形衝突判定
	for(auto& block : blocks){
		// プレイヤー
		if(player.GetCollider().CheckHit(block->GetCollider())){
			Vector3D playerEscapePosition = player.GetCollider().CollisionEscape(block->GetCollider(), 0.15f);
			player.HitObstacle(playerEscapePosition, Vector3DZero());
		}

		// vita達
		for(auto& vita : vitas){
			if(vita->GetCollider().CheckHit(block->GetCollider())){
				Vector3D vitaEscapePosition = vita->GetCollider().CollisionEscape(block->GetCollider(), 0.3f);
				vita->HitObstacle(vitaEscapePosition, Vector3DZero());
			}
		}

		// アイテム
		for(auto& item : items){
			if(item->GetCollider().CheckHit(block->GetCollider())){
				Vector3D itemEscapePosition = item->GetCollider().CollisionEscape(block->GetCollider(), 0.3f);
				item->HitObstacle(itemEscapePosition);
			}
		}
	}
}