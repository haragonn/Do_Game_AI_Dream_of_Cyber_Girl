/*==============================================================================
	[Framework.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_FRAMEWORK_H
#define INCLUDE_IDEA_FRAMEWORK_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace idea{
	struct FrameworkInitDesc{
		UINT width = 640U;
		UINT height = 480U;
		bool bWindowed = true;
		UINT fps = 60U;
		UINT frameSkipMax = 0U;
		bool bNetwork = false;
		bool bDebug = false;
		const char* pClassName = "idea";
	};

	class Scene;

	//------------------------------------------------------------------------------
	// クラス名　：Framework
	// クラス概要：ウィンドウの管理,ゲームのメインループを行う
	//------------------------------------------------------------------------------
	class Framework{
	public:

		// 初期化
		static bool Init(HINSTANCE hInst, int nCmdShow, const FrameworkInitDesc& fid);

		static bool Init(HINSTANCE hInst, int nCmdShow, UINT width, UINT height, bool bWindowed = true, UINT fps = 60U, UINT frameSkipMax = 0U, bool bNetwork = false, bool bDebug = false, const char* pClassName = "idea");

		// 終了処理
		static void UnInit();

		// シーンの実行
		static void Run(Scene* pInitScene);

		// ウィンドウタイトルの変更
		static bool SetWindowFormatText(const char* pFormat, ...);

		// FPSの取得
		static float GetFps();

		// 目標FPSの取得
		static UINT GetTargetFps();

		// FPS簡易表示の設定
		static void SetFpsSimpleDisplayFlag(bool bShow);

		// 更新処理にかかった時間の取得
		static double GetUpdateTime();

		// マウスカーソルの可視化の設定
		static void SetMouseCursorVisibleFlag(bool bOnWindowed, bool bOnFullscreen);
	};
};

#endif	// #ifndef INCLUDE_IDEA_FRAMEWORK_H