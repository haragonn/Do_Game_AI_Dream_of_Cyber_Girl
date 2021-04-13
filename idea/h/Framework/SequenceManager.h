/*==============================================================================
	[SequenceManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_SEQUENCEMANAGER_H
#define INCLUDE_IDEA_SEQUENCEMANAGER_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <atomic>

namespace idea{
	class Scene;

	//------------------------------------------------------------------------------
	// クラス名　：SequenceManager
	// クラス概要：シングルトン
	// 　　　　　　シーケンス制御を行うクラス
	//------------------------------------------------------------------------------
	class SequenceManager{
	public:
		// 唯一のインスタンスを返す
		static SequenceManager& Instance()
		{
			static SequenceManager s_Instance;
			return s_Instance;
		}

		// 初期化
		bool Init(Scene* pInitScene);
		
		// 終了処理
		void UnInit();

		// 更新
		int Update(bool bRedy);

		// 描画
		void Draw();

		// シーンの取得
		Scene* GetScenePtr(){ return pScene_; }

		// 次のシーンをセット
		void SetNextScenePtr(Scene* pNextScene){ pNextScene_ = pNextScene; }

	private:
		friend class FrameworkManager;

		// 現在のシーン
		Scene* pScene_ = nullptr;

		// 次のシーン
		Scene* pNextScene_ = nullptr;

		// 非同期読み込みスレッドのハンドル
		HANDLE hAsyncLoadThread_ = NULL;

		// 非同期読み込みの完了フラグ
		std::atomic<bool> bLoadCompleted_;

		// クリティカルセクション
		CRITICAL_SECTION cs_ = {};

		// 非同期読み込みスレッド
		friend unsigned __stdcall AsyncLoadThread(void* vp);

		// 非同期読み込み
		inline void AsyncLoad();

		SequenceManager();
		~SequenceManager();

		// コピーコンストラクタの禁止
		SequenceManager(const SequenceManager& src){}
		SequenceManager& operator=(const SequenceManager& src){}
	};
}

#endif	// #ifndef INCLUDE_IDEA_SEQUENCEMANAGER_H