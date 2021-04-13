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
		// 現在のシーンのポインタ取得
		Scene* GetScenePtr();

		// 次のシーンの設定
		void SetNextScenePtr(Scene* pNextScene);
	};
}

#endif	// #ifndef INCLUDE_IDEA_GAMEMANAGER_H