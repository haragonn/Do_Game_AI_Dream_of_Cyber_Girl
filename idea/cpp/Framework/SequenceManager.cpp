/*==============================================================================
	[SequenceManager.cpp]
														Author	:	Keigo Hara
==============================================================================*/
#include <exception>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>

#include "../../h/Framework/SequenceManager.h"
#include "../../h/Framework/Scene.h"
#include "../../h/Framework/AsyncLoadScene.h"
#include "../../h/2D/Actor2DManager.h"
#include "../../h/3D/Actor3DManager.h"
#include "../../h/Storage/StorageManager.h"
#include "../../h/Network/NetworkManager.h"
#include "../../h/Utility/ideaUtility.h"

using namespace idea;

namespace idea{
	//------------------------------------------------------------------------------
	// 非同期読み込みスレッド
	//------------------------------------------------------------------------------
	unsigned __stdcall AsyncLoadThread(void* vp)
	{
		SequenceManager* psq = (SequenceManager*)vp;

		EnterCriticalSection(&psq->cs_);

		psq->AsyncLoad();
		psq->bLoadCompleted_ = true;

		LeaveCriticalSection(&psq->cs_);

		_endthreadex(0);

		return 0U;
	}
}

//------------------------------------------------------------------------------
// コンストラクタ
//------------------------------------------------------------------------------
SequenceManager::SequenceManager() :
	pScene_(nullptr),
	pNextScene_(nullptr),
	hAsyncLoadThread_(nullptr),
	bLoadCompleted_(false){}

//------------------------------------------------------------------------------
// デストラクタ
//------------------------------------------------------------------------------
SequenceManager::~SequenceManager()
{
	UnInit();
}

//------------------------------------------------------------------------------
// 初期化
//------------------------------------------------------------------------------
bool SequenceManager::Init(Scene* pInitScene)
{
	if(!pInitScene){	// NULLチェック
		return false;
	}

	if(!pScene_){	// 既にシーンを持っていなければ
		pNextScene_ = pScene_ = pInitScene;	// シーンを登録

		StorageManager::Instance().CreatNullInstance();

		pScene_->Init();

		return true;
	}

	SafeDelete(pInitScene);	// シーンの破棄

	return false;
}

//------------------------------------------------------------------------------
// 終了処理
//------------------------------------------------------------------------------
void SequenceManager::UnInit()
{
	// スレッドの破棄
	if(hAsyncLoadThread_){
#ifdef _DEBUG
		// デバッグなら2分たったらで殺しちゃえ
		if(WaitForSingleObject(hAsyncLoadThread_, 2 * 60 * 1000) == WAIT_TIMEOUT){
			MessageBox(NULL, "AsyncLoadThreadが終了しません。強制終了します。", NULL, NULL);
			TerminateThread(hAsyncLoadThread_, FALSE);
		}
#else
		WaitForSingleObject(hAsyncLoadThread_, INFINITE);
#endif
		CloseHandle(hAsyncLoadThread_);
		hAsyncLoadThread_ = nullptr;
	}

	// シーンの破棄
	if(pScene_){
		pScene_->UnInit();

		StorageManager::Instance().ReleaseNullInstance();

		SafeDelete(pScene_);
	}
}

//------------------------------------------------------------------------------
// 更新
//------------------------------------------------------------------------------
int SequenceManager::Update(bool bRedy)
{
	if(pScene_){	// NULLチェック
		if(pScene_->bAsync_){	// 非同期読み込みシーンなら
			if(!bLoadCompleted_ && !hAsyncLoadThread_){
				// クリティカルセクション初期化
				InitializeCriticalSection(&cs_);
				hAsyncLoadThread_ = (HANDLE)_beginthreadex(NULL, 0, AsyncLoadThread, this, 0, NULL);	// 非同期読み込みスレッドの作成
			} else{
				// 非同期読み込みスレッドの監視
				DWORD ExitCode;
				GetExitCodeThread(hAsyncLoadThread_, &ExitCode);
				if(ExitCode != STILL_ACTIVE){
					CloseHandle(hAsyncLoadThread_);
					hAsyncLoadThread_ = nullptr;
				}
			}
		} else{
			bLoadCompleted_ = true;
		}

		pScene_->Update();	// シーンのアップデートから次のシーンを受け取る

		Actor2DManager::Instance().UpdatePrePosition();
		Actor3DManager::Instance().UpdatePrePosition();

		NetworkManager::Instance().WritingEnd();	// ネットワーク書き込み

		if(pNextScene_ == pScene_){	// シーン変わらなければそのまま続行
			return 0;
		}

		// 非同期読み込みスレッドの破棄
		if(hAsyncLoadThread_){
			if(WaitForSingleObject(hAsyncLoadThread_, INFINITE) == WAIT_TIMEOUT){ throw std::exception(); }
			CloseHandle(hAsyncLoadThread_);
			hAsyncLoadThread_ = nullptr;
		}

		// 異なるシーンを受け取ったら前のシーンを破棄してシーンを切り替える
		pScene_->UnInit();

		NetworkManager::Instance().WritingEnd();

		SafeDelete(pScene_);

		// ストレージの破棄要請チェック
		auto& sm = StorageManager::Instance();

		if(sm.IsUnLoadAllRequest()){
			// ストレージを全て破棄
			sm.UnLoadAll();
		} else{
			// 画像を全て破棄
			if(sm.GetUnLoadImageRequestSize()){
				sm.UnLoadImage();
			}

			// サウンドを全て破棄
			if(sm.GetUnLoadWaveRequestSize()){
				sm.UnLoadWave();
			}

			// OBJメッシュの破棄
			if(sm.GetUnLoadObjModelRequestSize()){
				sm.UnLoadObjModel();
			}

			// PMXメッシュの破棄
			if(sm.GetUnLoadPmxModelRequestSize()){
				sm.UnLoadPmxModel();
			}

			// PMXメッシュの破棄
			if(sm.GetUnLoadVmdMotionRequestSize()){
				sm.UnLoadVmdMotion();
			}
		}

		// シーン遷移
		if(pNextScene_ && bRedy){	// 次のシーンがあれば
			pScene_ = pNextScene_;

			if(pScene_->bAsync_){
				bLoadCompleted_ = false;
			}

			pScene_->Init();

			return 1;	// シーンを変更して続行
		}
	}

	StorageManager::Instance().ReleaseNullInstance();

	return -1;	// 中断
}

//------------------------------------------------------------------------------
// 描画
//------------------------------------------------------------------------------
void SequenceManager::Draw()
{
	if(pScene_){	// NULLチェック
		pScene_->Draw();
	}
}

//------------------------------------------------------------------------------
// 非同期読み込み
//------------------------------------------------------------------------------
void SequenceManager::AsyncLoad()
{
	if(!pScene_ || !pScene_->bAsync_){	// NULLチェック,非同期フラグチェック
		return;
	}

	if(dynamic_cast<AsyncLoadScene*>(pScene_)->bLoadCompleted_){
		return;
	}

	dynamic_cast<AsyncLoadScene*>(pScene_)->Load();

	dynamic_cast<AsyncLoadScene*>(pScene_)->bLoadCompleted_ = true;
}