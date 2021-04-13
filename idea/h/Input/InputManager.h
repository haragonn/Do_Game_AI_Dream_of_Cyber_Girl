/*==============================================================================
	[InputManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_INPUTMANAGER_H
#define INCLUDE_IDEA_INPUTMANAGER_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

namespace idea{
	//------------------------------------------------------------------------------
	// 構造体定義
	//------------------------------------------------------------------------------
#pragma pack(push,1)

	struct MouseInput{
		// ボタンの最大数
		static const int BUTTON_MAX = 4;

		// X座標
		int posX;

		// 前フレームのX座標
		int prePosX;

		// X座標の差分
		int differenceX;

		// Y座標
		int posY;

		// 前フレームのY座標
		int prePosY;

		// Y座標の差分
		int differenceY;

		// ボタンの押されたフレーム数
		unsigned int button[BUTTON_MAX];

		// ホイールの差分
		int wheel;
	};

	struct Joystick{
		// ボタンの最大数
		static const int BUTTON_MAX = 10;

		// ボタンが押されたかどうか
		bool button[BUTTON_MAX];

		// 上キーが押されたかどうか
		bool up;

		// 下キーが押されたかどうか
		bool down;

		// 左キーが押されたかどうか
		bool left;

		// 右キーが押されたかどうか
		bool right;

		// 左アナログスティックのX軸
		float lAxisX;

		// 左アナログスティックのY軸
		float lAxisY;

		// 右アナログスティックのX軸
		float rAxisX;

		// 右アナログスティックのY軸
		float rAxisY;

		// 左トリガー
		float lTrigger;

		// 右トリガー
		float rTrigger;
	};

#pragma pack(pop)

	//------------------------------------------------------------------------------
	// クラス名　：InputManager
	// クラス概要：シングルトン
	// 　　　　　　DirectInputデバイスの管理を行う
	// 　　　　　　キーボード,マウス,ジョイスティックの入力情報を更新,管理する
	//------------------------------------------------------------------------------
	class InputManager{
	public:
		static const int JOYSTIC_MAX = 4;	// ジョイスティックの最大数

		// 唯一のインスタンスを返す
		static InputManager& Instance()
		{
			static InputManager s_Instance;
			return s_Instance;
		}

		//========================================
		// DirectInput
		//========================================
		// 初期化
		bool Init(HINSTANCE hInst, HWND hWnd);

		// 終了処理
		void UnInit();

		// 全ての入力情報の更新
		bool UpdateAll();

		//========================================
		// キーボード
		//========================================
		// キーボードの入力情報の更新
		inline bool UpdateKeyState();

		// 指定したキーが押されているか
		bool IsKeyPress(const DWORD keyCode){ return !!(keyState_[keyCode] & 0x80); }

		//========================================
		// マウス
		//========================================
		// マウスの入力情報の更新
		inline bool UpdateMouseState();

		// マウスの入力情報の取得
		const MouseInput& GetMouse()const{ return mouseState_; }

		//========================================
		// ジョイスティック
		//========================================
		// ジョイスティックの入力情報の更新
		inline bool UpdateJoystick();

		// 刺さっているジョイスティックの数の取得
		int GetJoystickNum()const{ return joystickNum_; }

		// ジョイスティックを振動させる
		bool SetVibration(DWORD joystickID, float leftSpeed, float rightSpeed);

		// ジョイスティックの入力情報の取得
		const Joystick& GetJoystick(DWORD JoystickID);

	private:
		// ジョイスティック列挙のコールバック関数
		static BOOL CALLBACK EnumJoysticksCallback(LPCDIDEVICEINSTANCE pInst, LPVOID pvRef);

		// アナログスティックのコールバック関数
		static BOOL CALLBACK EnumAxesCallback(LPCDIDEVICEOBJECTINSTANCE pInst, LPVOID pvRef);

		// インスタンス
		HINSTANCE hInst_ = NULL;

		// ウィンドウハンドル
		HWND hWnd_ = NULL;

		// インターフェイス
		LPDIRECTINPUT8 pDInput_ = nullptr;

		// デバイスの能力
		DIDEVCAPS devCaps_ = {};

		// キーボードのデバイス
		LPDIRECTINPUTDEVICE8 pKeyboardDev_ = nullptr;

		// キーボードの入力情報
		BYTE keyState_[256] = {};

		// マウスのデバイス
		LPDIRECTINPUTDEVICE8 pMouseDev_ = nullptr;

		// マウスの入力情報
		MouseInput mouseState_;

		// ジョイスティックが使われていなかった場合の空の情報
		static Joystick s_NullJoystick_;

		// ジョイスティックのデバイス
		LPDIRECTINPUTDEVICE8 pJoystickDev_[JOYSTIC_MAX] = {};

		// 刺さってるジョイスティックの数
		DWORD joystickNum_ = 0U;

		// ジョイスティックデバイスの数
		DWORD joystickDevNum_ = 0U;

		// ジョイスティックの最大数
		DWORD maxJoystickNum_ = 0U;

		// ジョイスティックの入力情報
		Joystick* pJoystick_ = nullptr;

		InputManager();
		~InputManager(){ UnInit(); }

		// コピーコンストラクタの禁止
		InputManager(const InputManager& src){}
		InputManager& operator=(const InputManager& src){}
	};
}

#endif // #ifndef INCLUDE_IDEA_INPUTMANAGER_H