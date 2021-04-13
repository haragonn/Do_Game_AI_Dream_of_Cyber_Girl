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
	// �V�[��
	auto& scene = (*pScene_);

	// �v���C���[
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// vita���X�g
	auto vitas = scene.GetGameObjects<Vita>(VITA_LAYER);

	// �u���b�N���X�g
	auto blocks = scene.GetGameObjects<Block>(BLOCK_LAYER);

	// �A�C�e�����X�g
	auto items = scene.GetGameObjects<Item>(ITEM_LAYER);

	// ���s����
	auto& judg = (*scene.GetGameObject<Judg>(JUDG_LAYER));

	// ���^AI
	auto& metaAI = (*scene.GetGameObject<MetaAI>(META_AI_LAYER));

	// ����
	auto& voice = (*scene.GetGameObject<GameVoice>(GAME_VOICE_LAYER));

	// �v���C���[��AI�̏Փ˔���
	for(auto& vita : vitas){
		// ���Ŏ��͏Փ˔���Ȃ�
		if(vita->GetState() == VitaState::Disappearance){
			continue;
		}

		if(typeid(scene) != typeid(Preview)){
			// �Q�[�����[�h
			if(vita->GetCollider().CheckHit(player.GetCollider())){
				// vita�̍��W	
				Vector3D vitaPosition = vita->GetPosition();

				// �m�b�N�o�b�N�x�N�g��
				Vector3D backVector = vita->GetPosition() - player.GetPosition();

				// �����o������̍��W
				Vector3D vitaEscapePosition = vita->GetCollider().CollisionEscape(player.GetCollider(), 0.0f);

				// vita�̏Փ˃��A�N�V����
				if(vita->GetState() != VitaState::Appearance
					&& vita->GetState() != VitaState::Count){
					vita->HitObstacle(vitaEscapePosition, backVector.ReplaceY(0).Normalized() * 0.4f);
				}

				if(judg.IsPlayerOni()){
					// �v���C���[�@�ǐ�->����
					if(player.GetState() != PlayerState::Count){
						judg.SetPlayerOniFlag(false);

						metaAI.OniChange();

						vita->Count();

						// �G�t�F�N�g����
						auto& effect = (*scene.AddGameObject<TouchEffect>(TOUCH_EFFECT_LAYER));

						// �G�t�F�N�g�������W
						Vector3D touchPosition = vita->GetPosition();
						touchPosition.y += 5.0f;

						effect.SetPosition(touchPosition);
						effect.SetColor(ideaColor::SOFT_RED);

						voice.PlayerTouch();
					}
				} else{
					// �v���C���[�@����->�ǐ�
					// vita�̍��W	
					Vector3D playerPosition = player.GetPosition();

					// �m�b�N�o�b�N�x�N�g��
					Vector3D backVector = player.GetPosition() - vita->GetPosition();

					// �����o������̍��W
					Vector3D playerEscapePosition = player.GetCollider().CollisionEscape(vita->GetCollider(), 0.0f);

					// vita�̏Փ˃��A�N�V����
					player.HitObstacle(playerEscapePosition, backVector.ReplaceY(0).Normalized() * 0.4f);

					if(!(vita->GetState() == VitaState::Count
						|| vita->GetState() == VitaState::Appearance)){
						judg.SetPlayerOniFlag(true);

						metaAI.OniChange();

						// �^�b�`����AI�ȊO�͏���
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

						// �G�t�F�N�g����
						auto& effect = (*scene.AddGameObject<TouchEffect>(TOUCH_EFFECT_LAYER));

						// �G�t�F�N�g�������W
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
			// �v���r���[���[�h
			if(vita->GetCollider().CheckHit(player.GetCollider())){
				// vita
				Vector3D vitaEscapePosition = vita->GetCollider().CollisionEscape(player.GetCollider(), 0.0f);
				vita->HitObstacle(vitaEscapePosition, Vector3DZero());

				// �v���C���[
				Vector3D playerEscapePosition = player.GetCollider().CollisionEscape(vita->GetCollider(), 0.0f);
				player.HitObstacle(playerEscapePosition, Vector3DZero());
			}
		}
	}

	// AI���m�̏Փ�
	for(auto& vita : vitas){
		// ���Ŏ��͏Փ˔���Ȃ�
		if(vita->GetState() == VitaState::Disappearance){
			continue;
		}

		for(auto& vita2 : vitas){
			// ���Ŏ��͏Փ˔���Ȃ�
			if(vita2->GetState() == VitaState::Disappearance){
				continue;
			}

			// �����Ƃ͏Փ˔���Ȃ�
			if(vita == vita2){
				continue;
			}

			if(vita->GetCollider().CheckHit(vita2->GetCollider())){
				if(typeid(scene) != typeid(Preview)){
					// �Q�[�����[�h
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
					// �v���r���[���[�h
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

	// �A�C�e���̎擾
	for(auto& item : items){
		if(player.GetCollider().CheckHit(item->GetCollider())){
			item->SetDestroy();

			player.Doping();

			metaAI.GetItem();

			break;
		}
	}

	// �n�`�Փ˔���
	for(auto& block : blocks){
		// �v���C���[
		if(player.GetCollider().CheckHit(block->GetCollider())){
			Vector3D playerEscapePosition = player.GetCollider().CollisionEscape(block->GetCollider(), 0.15f);
			player.HitObstacle(playerEscapePosition, Vector3DZero());
		}

		// vita�B
		for(auto& vita : vitas){
			if(vita->GetCollider().CheckHit(block->GetCollider())){
				Vector3D vitaEscapePosition = vita->GetCollider().CollisionEscape(block->GetCollider(), 0.3f);
				vita->HitObstacle(vitaEscapePosition, Vector3DZero());
			}
		}

		// �A�C�e��
		for(auto& item : items){
			if(item->GetCollider().CheckHit(block->GetCollider())){
				Vector3D itemEscapePosition = item->GetCollider().CollisionEscape(block->GetCollider(), 0.3f);
				item->HitObstacle(itemEscapePosition);
			}
		}
	}
}