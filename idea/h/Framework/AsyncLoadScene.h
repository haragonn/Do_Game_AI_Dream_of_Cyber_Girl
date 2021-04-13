/*==============================================================================
	[AsyncLoadScene.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_ASYNCLOADSCENE_H
#define INCLUDE_IDEA_ASYNCLOADSCENE_H

#include "Scene.h"
#include <atomic>

namespace idea{
	//------------------------------------------------------------------------------
	// クラス名　：AsyncScene
	// クラス概要：非同期読み込み付きのゲームシーンクラスの基底となるクラス
	//------------------------------------------------------------------------------
	class AsyncLoadScene : public Scene{
	public:
		AsyncLoadScene() : bLoadCompleted_(false){ bAsync_ = true; }
		virtual ~AsyncLoadScene(){}

		// 非同期読み込み
		virtual void Load() = 0;

		// 非同期読み込みが完了したか
		bool IsLoadCompleted()const{ return bLoadCompleted_; }

	private:
		friend class SequenceManager;

		// 非同期読み込み完了フラグ
		std::atomic<bool> bLoadCompleted_;
	};
}

#endif	// #ifndef INCLUDE_IDEA_ASYNCLOADSCENE_H