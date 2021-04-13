/*==============================================================================
	[GameManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_GAMEMANAGER_H
#define INCLUDE_IDEA_GAMEMANAGER_H

namespace idea{
	class Scene;

	class GameManager{
	public:
		// ���݂̃V�[���̃|�C���^�擾
		Scene* GetScenePtr();

		// ���̃V�[���̐ݒ�
		void SetNextScenePtr(Scene* pNextScene);
	};
}

#endif	// #ifndef INCLUDE_IDEA_GAMEMANAGER_H