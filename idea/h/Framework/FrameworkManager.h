/*==============================================================================
	[FrameworkManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_FLAMEWORKMANAGER_H
#define INCLUDE_IDEA_FLAMEWORKMANAGER_H

#include "../Framework/BaseWindow.h"

#include <atomic>

namespace idea{
	class Scene;
	//------------------------------------------------------------------------------
	// クラス名　：FrameworkManager
	// クラス概要：シングルトン
	// 　　　　　　ウィンドウの管理,ゲームのメインループを行う
	//------------------------------------------------------------------------------
	class FrameworkManager : public BaseWindow{
	public:
		// 唯一のインスタンスを返す
		static FrameworkManager& Instance()
		{
			static FrameworkManager s_Instance;
			return s_Instance;
		}

		// 初期化
		bool Init(HINSTANCE hInst, int nCmdShow, UINT width, UINT height, bool bWindowed = true, UINT fps = 60U, UINT frameSkipMax = 0U, bool bNetwork = false, bool bDebug = false, const char* pClassName = "idea");

		// 終了処理
		void UnInit();

		// シーンの実行
		void Run(Scene* pInitScene);

		// FPSの取得
		float GetFps(){ return fps_; }

		// 目標FPSの取得
		UINT GetTargetFps()const{ return targetFps_; }

		// FPS簡易表示の設定
		void SetFpsSimpleDisplayFlag(bool bShow){ bFpsShow_ = bShow; }

		// 更新処理にかかった時間の取得
		double GetUpdateTime()const{ return updateTime_; }

		// マウスカーソルの可視化の設定
		void SetMouseCursorVisibleFlag(bool bOnWindowed, bool bOnFullscreen);

	private:
		// インスタンスハンドル
		HINSTANCE hInst_ = NULL;

		// スクリーンの幅
		UINT width_ = 0U;

		// スクリーンの高さ
		UINT height_ = 0U;

		// ウィンドウモード
		bool bWindowed_ = false;

		// 実行準備完了
		std::atomic<bool> bReady_;

		// 初期シーン
		std::atomic<Scene*> pInitScene_;

		// 終了フラグ
		bool bExit_ = false;

		// ウィンドウ時のマウスカーソル表示設定
		bool bOnWindowed_ = false;

		// フルスクリーン時のマウスカーソル表示設定
		bool bOnFullscreen_ = false;

		// ウィンドウモードの切り替え要請
		std::atomic<bool> bChangeDispRequest_;

		// ウインドウモードの切り替えの完了
		std::atomic<bool> bChangeDispReady_;

		// 目標のFPS
		std::atomic<UINT> targetFps_;

		// 現在のFPS
		float fps_ = 0.0f;

		// FPSの簡易表示フラグ
		bool bFpsShow_ = false;

		// 更新処理にかかった時間
		std::atomic<double> updateTime_;

		// 最大フレームスキップ数
		std::atomic<UINT> frameSkipMax_;

		// フレームスキップ回数
		std::atomic<UINT> frameSkipCount_;

		// ストレージの使用可能フラグ
		bool bStorage_ = false;

		// デバッグ機能使用フラグ
		std::atomic<bool> bDebug_;

		// メインループスレッドのハンドル
		HANDLE hMainLoopThread_ = NULL;

		// クリティカルセクション
		CRITICAL_SECTION cs_ = {};

		// メインループスレッド
		friend unsigned __stdcall MainLoopThread(void* vp);

		// メインループ
		inline void MainLoop();

		// ウィンドウモードの切り替え
		inline void ChangeDisplayMode();

		// ウィンドウが閉じられた時
		LRESULT OnClose()override;

		// ウィンドウが破棄される時
		LRESULT OnDestroy()override;

		// キーが入力された時
		LRESULT OnKeyDown()override;

		// システムキーが入力された時
		LRESULT OnSysKeyDown()override;

		// ウィンドウ内でマウスカーソルが動いた時
		LRESULT OnSetCursor()override;

		FrameworkManager();
		~FrameworkManager(){}

		// コピーコンストラクタの禁止
		FrameworkManager(FrameworkManager& src){}
		FrameworkManager& operator=(FrameworkManager& src){}
	};
}

#endif // #ifndef INCLUDE_IDEA_FLAMEWORKMANAGER_H