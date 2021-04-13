/*==============================================================================
	[FrameworkManager.cpp]
														Author	:	Keigo Hara
==============================================================================*/
#include <stdio.h>
#include <exception>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>

#include "../../h/Framework/FrameworkManager.h"
#include "../../h/Framework/SequenceManager.h"
#include "../../h/Framework/Scene.h"
#include "../../h/Framework/GraphicManager.h"
#include "../../h/2D/Actor2DManager.h"
#include "../../h/2D/SpriteManager.h"
#include "../../h/2D/SpriteInstancingManager.h"
#include "../../h/3D/Actor3DManager.h"
#include "../../h/3D/ObjectManager.h"
#include "../../h/Input/InputManager.h"
#include "../../h/Sound/SoundManager.h"
#include "../../h/Utility/ImGuiManager.h"
#include "../../h/Utility/DebugManager.h"
#include "../../h/Network/NetworkManager.h"
#include "../../h/Storage/StorageManager.h"
#include "../../h/Framework/FadeManager.h"
#include "../../h/Utility/Timer.h"
#include "../../h/Utility/ideaUtility.h"

const DWORD WINDOW_STYLE = WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME);
const DWORD FULL_SCREEN_STYLE = WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP;

using namespace idea;

namespace idea{
	//------------------------------------------------------------------------------
	// メインループスレッド
	//------------------------------------------------------------------------------
	unsigned __stdcall MainLoopThread(void* vp)
	{
		FrameworkManager* pfm = (FrameworkManager*)vp;

		EnterCriticalSection(&pfm->cs_);

		pfm->MainLoop();

		LeaveCriticalSection(&pfm->cs_);

		_endthreadex(0);

		return 0U;
	}
}

//------------------------------------------------------------------------------
// コンストラクタ
//------------------------------------------------------------------------------
FrameworkManager::FrameworkManager() :
	hInst_(nullptr),
	width_(640U),
	height_(480U),
	bWindowed_(true),
	bReady_(false),
	pInitScene_(nullptr),
	bExit_(false),
	bOnWindowed_(true),
	bOnFullscreen_(false),
	bChangeDispRequest_(false),
	bChangeDispReady_(true),
	targetFps_(60U),
	fps_(60.0f),
	bFpsShow_(false),
	updateTime_(0U),
	frameSkipMax_(0U),
	frameSkipCount_(0U),
	bStorage_(false),
	bDebug_(false),
	hMainLoopThread_(nullptr){}

//------------------------------------------------------------------------------
// 初期化
//------------------------------------------------------------------------------
bool FrameworkManager::Init(HINSTANCE hInst, int nCmdShow, UINT width, UINT height, bool bWindowed, UINT fps, UINT frameSkipMax, bool bNetwork, bool bDebug, const char* pClassName)
{
	SetLeakCheckFlag();	// メモリリークチェックのフラグをオンに

	if(bReady_){	// 初期化済みなら失敗
		return false;
	}

	// 引数を反映
	hInst_ = hInst;
	width_ = width;
	height_ = height;
	bWindowed_ = true;
	targetFps_ = fps;
	fps_ = (float)fps;
	frameSkipMax_ = frameSkipMax;
	frameSkipCount_ = 0;
	bDebug_ = bDebug;

	//========================================
	// ウィンドウ
	//========================================
	// ウィンドウクラスの登録
	wcex_.cbSize = sizeof(WNDCLASSEX);						// 構造体サイズ
	wcex_.style = CS_HREDRAW | CS_VREDRAW;					// ウィンドウクラススタイル
	wcex_.lpfnWndProc = (WNDPROC)WndProc;					// ウィンドウプロシージャ
	wcex_.cbClsExtra = 0;									// 補助領域サイズ
	wcex_.cbWndExtra = 0;									// 補助領域サイズ
	wcex_.hInstance = hInst_;								// インスタンスハンドル
	wcex_.hIcon = LoadIcon(NULL, IDI_APPLICATION);			// アイコン
	wcex_.hCursor = LoadCursor(NULL, IDC_ARROW);			// マウスカーソル
	wcex_.hbrBackground = (HBRUSH)(COLOR_WINDOWTEXT + 1);	// クライアント領域背景色
	wcex_.lpszMenuName = NULL;								// ウィンドウメニュー
	wcex_.lpszClassName = pClassName;						// ウィンドウクラス名
	wcex_.hIconSm = LoadIcon(NULL, IDI_APPLICATION);		// 小アイコン
	if(!RegisterClassEx(&wcex_)){ return false; }

	// ウィンドウの作成
	LONG tmpWidth = (LONG)width_;
	LONG tmpHeight = (LONG)height_;
	RECT cr = { 0, 0, tmpWidth, tmpHeight };
	AdjustWindowRect(&cr, WINDOW_STYLE, FALSE);
	int ww = cr.right - cr.left;
	int wh = cr.bottom - cr.top;
	RECT dr;
	GetWindowRect(GetDesktopWindow(), &dr);
	int dw = dr.right - dr.left;
	int dh = dr.bottom - dr.top;
	int wx = (ww > dw) ? 0 : (dw - ww) >> 1;
	int wy = (wh > dh) ? 0 : (dh - wh) >> 1;
	hWnd_ = CreateWindow(wcex_.lpszClassName, pClassName, WINDOW_STYLE, wx, wy, ww, wh, NULL, NULL, hInst_, this);
	if(!hWnd_){
		return false;
	}

	//========================================
	// DirectX
	//========================================
	if(!GraphicManager::Instance().Init(hWnd_, width_, height_, bWindowed_, targetFps_)){
		MessageBox(hWnd_, "DirectX11を初期化できませんでした", NULL, NULL);
		return false;
	}

	if(!SpriteManager::Instance().Init()){
		MessageBox(hWnd_, "2Dシェーダーを初期化できませんでした", NULL, NULL);
		return false;
	}

	if(!SpriteInstancingManager::Instance().Init()){
		MessageBox(hWnd_, "2Dシェーダーを初期化できませんでした(2)", NULL, NULL);
		return false;
	}

	if(!ObjectManager::Instance().Init()){
		MessageBox(hWnd_, "3Dシェーダーを初期化できませんでした", NULL, NULL);
		return false;
	}

	if(!InputManager::Instance().Init(hInst_, hWnd_)){
		MessageBox(hWnd_, "DirectInputを初期化できませんでした", NULL, NULL);
		return false;
	}

	if(!SoundManager::Instance().Init()){
		MessageBox(hWnd_, "XAudio2を初期化できませんでした", NULL, NULL);
		return false;
	}

	//========================================
	// フェード
	//========================================
	FadeManager::Instance().Init((float)width_, (float)height_);

	//========================================
	// Winsock
	//========================================
	if(bNetwork){
		if(!NetworkManager::Instance().Init()){
			MessageBox(hWnd_, "Winsockを初期化できませんでした", NULL, NULL);
			return false;
		}
	}

	//========================================
	// ImGui
	//========================================
	if(!ImGuiManager::Instance().Init()){
		MessageBox(hWnd_, "ImGuiを初期化できませんでした", NULL, NULL);
		return false;
	}

	if(bDebug){
		DebugManager::Instance().Init();
	}

	// ウィンドウの表示
	if(!bWindowed){
		bChangeDispRequest_ = true;
	}
	ValidateRect(hWnd_, 0);			// WM_PAINTが呼ばれないようにする
	UpdateWindow(hWnd_);			// クライアント領域の更新
	ShowWindow(hWnd_, nCmdShow);	// 表示状態の設定

	bStorage_ = true;				// ストレージの使用可能フラグをオンに
	bReady_ = true;					// 準備フラグをオンに

	return true;
}

//------------------------------------------------------------------------------
// 終了処理
//------------------------------------------------------------------------------
void FrameworkManager::UnInit()
{
	if(!bReady_){	// 初期化していなければ失敗
		return;
	}

	bReady_ = false;	// 準備フラグをオフに

	if(hMainLoopThread_){
		// メインループスレッドの終了を待つ
#ifdef _DEBUG
		// デバッグなら2分たったら殺しちゃえ
		if(WaitForSingleObject(hMainLoopThread_, 2 * 60 * 1000) == WAIT_TIMEOUT){
			MessageBox(hWnd_, "MainLoopThreadが終了しません。強制終了します。", NULL, NULL);
			TerminateThread(hMainLoopThread_, FALSE);
		}
#else
		WaitForSingleObject(hMainLoopThread_, INFINITE);
#endif
		CloseHandle(hMainLoopThread_);
		hMainLoopThread_ = nullptr;
	}

	// 各マネージャの終了処理
	StorageManager::Instance().UnLoadAll();
	ObjectManager::Instance().UnInit();
	SpriteManager::Instance().UnInit();
	SpriteInstancingManager::Instance().UnInit();
	DebugManager::Instance().UnInit();
	ImGuiManager::Instance().UnInit();
	GraphicManager::Instance().UnInit();
	SoundManager::Instance().UnInit();
	InputManager::Instance().UnInit();
	NetworkManager::Instance().UnInit();
}

//------------------------------------------------------------------------------
// メインループ
//------------------------------------------------------------------------------
void FrameworkManager::MainLoop()
{
	SequenceManager& sq = SequenceManager::Instance();	// シーケンスのインスタンス
	GraphicManager& gm = GraphicManager::Instance();	// グラフィックマネージャのインスタンス
	FadeManager& fm = FadeManager::Instance();			// フェードのインスタンス
	ImGuiManager& im = ImGuiManager::Instance();		// ImGuiマネージャのインスタンス
	DebugManager& dm = DebugManager::Instance();		// デバッグマネージャのインスタンス

	if(!sq.Init(pInitScene_)){	// シーケンスの初期化
		sq.UnInit();
		return;
	}

	Timer tmr;				// タイマー
	Timer tmrFps;			// FPS計測用タイマー
	Timer tmrUpdate;
	double frameTime = 1000.0 / targetFps_;	// 1フレームの単位時間
	unsigned long long frameCount = 0U;		// フレーム数
	UINT fpsCount = 0U;		// FPSカウント

	tmr.SetStartTime();		// タイマー開始
	tmrFps.SetStartTime();	//

	while(bReady_){
		if(bChangeDispRequest_){	// ディスプレイモード切り替えリクエスト受け取り
			bChangeDispRequest_ = false;
			bChangeDispReady_ = false;
		}

		if(!bChangeDispReady_){		// ディスプレイモードを切り替えている最中なら中止
			frameCount = 1U;
			fpsCount = 0U;
			frameSkipCount_ = 0U;

			tmr.SetStartTime();
			tmrFps.SetStartTime();

			continue;
		}

		// FPSの計測
		if(fpsCount == targetFps_){
			fps_ = (float)(targetFps_ * 1000.0 / tmrFps.GetHighPrecisionElapsed());

			fpsCount = 0U;

			tmrFps.SetStartTime();
		}

		double elapsed = tmr.GetHighPrecisionElapsed();

		if(elapsed > frameTime * (frameCount + 1) || NetworkManager::Instance().GetSkipFlag()){	// 目標の時間がたったら
			tmrUpdate.SetStartTime();

			im.Update();	// imGuiの更新

			InputManager::Instance().UpdateAll();	// 入力

			int sqTemp = sq.Update(bReady_);	// 更新

			// 終了要請
			if(sqTemp == -1){
				break;
			}

			fm.Update();	// フェードの更新

			if(bDebug_){
				dm.Update(sq.bLoadCompleted_);	// デバッグ情報の更新
			}

			updateTime_ = tmrUpdate.GetHighPrecisionElapsed();

			// 描画
			if(!(frameSkipMax_ > frameSkipCount_ && updateTime_ > frameTime || NetworkManager::Instance().GetSkipFlag())){

				if(gm.BeginScene()){
					gm.EndMask();	// マスクのクリーン

					sq.Draw();		// シーケンスの描画

					fm.Draw();		// フェードの描画

					im.Draw();		// ImGuiの描画

					gm.EndScene();

					frameSkipCount_ = 0U;
				}
			} else{
				im.Draw();		// ImGuiの描画

				++frameSkipCount_;
			}

			if(sqTemp == 1){	// シーンの切り替え時フレームカウントを初期化する
				frameCount = 0U;
				fpsCount = 0U;
				frameSkipCount_ = 0U;

				// デバッグ情報の更新
				if(bDebug_){
					dm.RefreshTime();
				}

				// 時間計測開始
				tmr.SetStartTime();
				tmrFps.SetStartTime();

				continue;
			}

			// フレームカウントの更新
			if(elapsed < frameTime * (frameCount + 2)){
				++frameCount;
			} else{
				frameCount = static_cast<unsigned long long>(elapsed / frameTime);
			}

			++fpsCount;
		} else{
			NetworkManager::Instance().WritingEnd();
			Sleep(1);	// 目標の時間が立っていなければ休む
		}

		if(NetworkManager::Instance().GetSkipFlag()){
			NetworkManager::Instance().WritingEnd();
			Sleep(1);
		}
	}

	sq.UnInit();
}

//------------------------------------------------------------------------------
// シーンの実行
//------------------------------------------------------------------------------
void FrameworkManager::Run(Scene* pInitScene)
{
	if(bExit_){	// Run()の再呼び出しの処理
		SafeDelete(pInitScene);
		return;
	}

	if(!bReady_ || !pInitScene){	// 準備はいいか,シーンのNULLチェック
		return;
	}

	pInitScene_ = pInitScene;

	DWORD ExitCode;

	// クリティカルセクション初期化
	InitializeCriticalSection(&cs_);

	// メインループは別スレッドに投げる
	hMainLoopThread_ = (HANDLE)_beginthreadex(NULL, 0, MainLoopThread, this, 0, NULL);

	// メインスレッドではメッセージを処理,メインループスレッド,デバイスも監視する
	while(ProcessMessage()){
		// ディスプレイモードの切り替え
		if(!bChangeDispReady_){
			ChangeDisplayMode();
		}

		ImGuiManager::Instance().SetWndProcHandler(hWnd_, msg_, wp_, lp_);	// ImGuiのプロシージャ

		// 簡易FPS表示
		if(bFpsShow_){
			char temp[16] = {};
			sprintf_s(temp, " fps[%2.1f]", static_cast<int>(fps_ * 10) * 0.1f);
			char buf[300] = {};
			strcpy_s(buf, 300, title_);
			strcat_s(buf, 300, temp);
			SetWindowText(hWnd_, buf);
		}

		// メインループスレッドとウィンドウの監視
		if(hMainLoopThread_){
			GetExitCodeThread(hMainLoopThread_, &ExitCode);
			if(ExitCode != STILL_ACTIVE){
				CloseHandle(hMainLoopThread_);
				hMainLoopThread_ = nullptr;
				break;
			} else{
				Sleep(30);
			}
		} else{
			break;
		}
	}

	bExit_ = true;

	UnInit();
}

//------------------------------------------------------------------------------
// マウスカーソルの可視化の設定
//------------------------------------------------------------------------------
void FrameworkManager::SetMouseCursorVisibleFlag(bool bOnWindowed, bool bOnFullscreen)
{
	// フラグの反映
	bOnWindowed_ = bOnWindowed;
	bOnFullscreen_ = bOnFullscreen;
}

//------------------------------------------------------------------------------
// ウィンドウモードの切り替え
//------------------------------------------------------------------------------
void FrameworkManager::ChangeDisplayMode()
{
	bWindowed_ = !bWindowed_;	// ウィンドウモードフラグを切り替える

	GraphicManager::Instance().ChangeDisplayMode(bWindowed_);	// グラフィックマネージャにウィンドウモード切り替えを要請

	// ウィンドウの変更
	if(bWindowed_){	// ウインドウ時
		LONG width = (LONG)width_;
		LONG height = (LONG)height_;
		RECT cr = { 0, 0, width, height };
		AdjustWindowRect(&cr, WINDOW_STYLE, FALSE);

		int ww = cr.right - cr.left;
		int wh = cr.bottom - cr.top;
		RECT dr;
		GetWindowRect(GetDesktopWindow(), &dr);
		int dw = dr.right - dr.left;
		int dh = dr.bottom - dr.top;
		int wx = (ww > dw) ? 0 : (dw - ww) >> 1;
		int wy = (wh > dh) ? 0 : (dh - wh) >> 1;

		SetWindowLong(hWnd_, GWL_STYLE, WINDOW_STYLE);
		SetWindowPos(hWnd_, HWND_NOTOPMOST, wx, wy, ww, wh, SWP_SHOWWINDOW);
	} else{	// フルスクリーン時
		RECT dr;
		GetWindowRect(GetDesktopWindow(), &dr);

		int dw = dr.right - dr.left;
		int dh = dr.bottom - dr.top;

		SetWindowLong(hWnd_, GWL_STYLE, FULL_SCREEN_STYLE);
		SetWindowPos(hWnd_, HWND_TOPMOST, 0, 0, dw, dh, SWP_SHOWWINDOW);
	}

	// リクエスト解消
	bChangeDispRequest_ = false;
	bChangeDispReady_ = true;
}

//------------------------------------------------------------------------------
// ウィンドウが閉じられた時
//------------------------------------------------------------------------------
LRESULT FrameworkManager::OnClose()
{
	DestroyWindow(hWnd_);

	return 0;
}

//------------------------------------------------------------------------------
// ウィンドウが破棄される時
//------------------------------------------------------------------------------
LRESULT FrameworkManager::OnDestroy()
{
	UnInit();

	PostQuitMessage(0);	// 終了メッセージを送る

	return 0;
}

//------------------------------------------------------------------------------
// キーが入力された時
//------------------------------------------------------------------------------
LRESULT FrameworkManager::OnKeyDown(){
	if(wp_ == VK_ESCAPE){	// エスケープキーが押されたら
		if(bWindowed_){	// ウィンドウ時
#ifdef _DEBUG
			// デバッグ時はいきなり終了
			SendMessage(hWnd_, WM_CLOSE, 0, 0);	// 終了
			return 0;
#endif
			// 終了確認を促す
			char temp[256] = {};
			sprintf_s(temp, "『%s』を終了しますか？", wcex_.lpszClassName);
			if(MessageBox(hWnd_, temp, "終了確認", MB_OKCANCEL) == IDOK){
				SendMessage(hWnd_, WM_CLOSE, 0, 0);	// 終了
			}
		} else if(!bChangeDispRequest_){	// フルスクリーン時

			bChangeDispRequest_ = true;	// ウィンドウモードの切り替え
		}

		return 0;
	} else{
		return DefaultProc();
	}
}

//------------------------------------------------------------------------------
// システムキーが入力された時
//------------------------------------------------------------------------------
LRESULT FrameworkManager::OnSysKeyDown()
{
	// Alt+エンターキーが押されたらウィンドウモードの切り替え
	if(wp_ != VK_RETURN){
		return DefaultProc();
	}

	if(!NetworkManager::Instance().IsConnect() && !bChangeDispRequest_){
		bChangeDispRequest_ = true;
	}

	return 0;
}

//------------------------------------------------------------------------------
// ウィンドウ内でマウスカーソルが動いた時
//------------------------------------------------------------------------------
LRESULT FrameworkManager::OnSetCursor()
{
	// フラグに合わせて表示を切り替える
	if(bWindowed_ ? !bOnWindowed_ : !bOnFullscreen_){
		SetCursor(NULL);	// マウスを非表示

		return 0;
	} else{
		return DefaultProc();
	}
}