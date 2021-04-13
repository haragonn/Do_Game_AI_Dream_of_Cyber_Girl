/*==============================================================================
	[BaseWindow.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_BASEWINDOW_H
#define INCLUDE_IDEA_BASEWINDOW_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace idea{
	//------------------------------------------------------------------------------
	// クラス名　：BaseWindow
	// クラス概要：ウィンドウを扱うWindowアプリケーションの基底となるクラス
	// 　　　　　　主にメッセージ管理を行う
	//------------------------------------------------------------------------------
	class BaseWindow{
	public:
		BaseWindow();
		virtual ~BaseWindow(){}
		
		// ウィンドウタイトルの変更
		bool SetWindowFormatText(const char* pFormat, ...);

	protected:
		// ウィンドウハンドル
		HWND hWnd_ = NULL;
		
		// ウィンドウクラス
		WNDCLASSEX wcex_ = {};
		
		// メッセージ
		UINT msg_ = 0U;
		
		// パラメータ
		WPARAM wp_ = NULL;
		
		// パラメータ
		LPARAM lp_ = NULL;
		
		// ウインドウタイトル
		char title_[256] = {};

		// メッセージの監視
		bool ProcessMessage();

		// ウィンドウプロシージャ
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
		
		// メッセージの管理
		virtual inline LRESULT MessageProc();
		
		// ウィンドウが生成された時
		virtual LRESULT OnCreate()		{ return DefaultProc(); }
		
		// ウィンドウが閉じられた時
		virtual LRESULT OnClose()		{ return DefaultProc(); }
		
		// ウィンドウが破棄される時
		virtual LRESULT OnDestroy()		{ PostQuitMessage(0); return 0; }
		
		// ウィンドウのアクティブ化,非アクティブ化の通知
		virtual LRESULT OnActivate()	{ return DefaultProc(); }
		
		// キーボードフォーカスを得た時
		virtual LRESULT OnSetFocus()	{ return DefaultProc(); }
		
		// キーボードフォーカスを失った時
		virtual LRESULT OnKillFocus()	{ return DefaultProc(); }
		
		// ウィンドウが移動された時
		virtual LRESULT OnMove()		{ return DefaultProc(); }
		
		// ウィンドウのサイズが変更された時
		virtual LRESULT OnSize()		{ return DefaultProc(); }
		
		// 描画する時
		virtual LRESULT OnPaint()		{ return DefaultProc(); }
		
		// キーが入力された時
		virtual LRESULT OnKeyDown()		{ return DefaultProc(); }
		
		// システムキーが入力された時
		virtual LRESULT OnSysKeyDown()	{ return DefaultProc(); }
		
		// ウィンドウ内でマウスカーソルが動いた時
		virtual LRESULT OnSetCursor()	{ return DefaultProc(); }
		
		// デフォルトの処理
		inline LRESULT DefaultProc()	{ return DefWindowProc(hWnd_, msg_, wp_, lp_); }
	};
}

#endif // #ifndef INCLUDE_IDEA_BASEWINDOW_H