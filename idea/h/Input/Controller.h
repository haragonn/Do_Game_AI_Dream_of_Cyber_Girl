/*==============================================================================
	[Controller.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_CONTROLLER_H
#define INCLUDE_IDEA_CONTROLLER_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "../Utility/ideaMath.h"
#include <vector>

namespace idea{
	// �L�[�R�[�h
	enum class KeyCode{
		KEY_CODE_UNUSED = 0x0,
		ESCAPE = 0x01,
		_1 = 0x02,
		_2 = 0x03,
		_3 = 0x04,
		_4 = 0x05,
		_5 = 0x06,
		_6 = 0x07,
		_7 = 0x08,
		_8 = 0x09,
		_9 = 0x0A,
		_0 = 0x0B,
		MINUS = 0x0C,
		EQUALS = 0x0D,
		BACKSPACE = 0x0E,
		TAB = 0x0F,
		Q = 0x10,
		W = 0x11,
		E = 0x12,
		R = 0x13,
		T = 0x14,
		Y = 0x15,
		U = 0x16,
		I = 0x17,
		O = 0x18,
		P = 0x19,
		LBRACKET = 0x1A,
		RBRACKET = 0x1B,
		RETURN = 0x1C,
		LCONTROL = 0x1D,
		A = 0x1E,
		S = 0x1F,
		D = 0x20,
		F = 0x21,
		G = 0x22,
		H = 0x23,
		J = 0x24,
		K = 0x25,
		L = 0x26,
		SEMICOLON = 0x27,
		APOSTROPHE = 0x28,
		GRAVE = 0x29,
		LSHIFT = 0x2A,
		BACKSLASH = 0x2B,
		Z = 0x2C,
		X = 0x2D,
		C = 0x2E,
		V = 0x2F,
		B = 0x30,
		N = 0x31,
		M = 0x32,
		COMMA = 0x33,
		PERIOD = 0x34,
		SLASH = 0x35,
		RSHIFT = 0x36,
		NUMPADSTAR = 0x37,
		LALT = 0x38,
		SPACE = 0x39,
		CAPSLOCK = 0x3A,
		F1 = 0x3B,
		F2 = 0x3C,
		F3 = 0x3D,
		F4 = 0x3E,
		F5 = 0x3F,
		F6 = 0x40,
		F7 = 0x41,
		F8 = 0x42,
		F9 = 0x43,
		F10 = 0x44,
		NUMLOCK = 0x45,
		SCROLL = 0x46,
		NUMPAD7 = 0x47,
		NUMPAD8 = 0x48,
		NUMPAD9 = 0x49,
		NUMPADMINUS = 0x4A,
		NUMPAD4 = 0x4B,
		NUMPAD5 = 0x4C,
		NUMPAD6 = 0x4D,
		NUMPADPLUS = 0x4E,
		NUMPAD1 = 0x4F,
		NUMPAD2 = 0x50,
		NUMPAD3 = 0x51,
		NUMPAD0 = 0x52,
		NUMPADPERIOD = 0x53,
		OEM_102 = 0x56,
		F11 = 0x57,
		F12 = 0x58,
		F13 = 0x64,
		F14 = 0x65,
		F15 = 0x66,
		KANA = 0x70,
		ABNT_C1 = 0x73,
		CONVERT = 0x79,
		NOCONVERT = 0x7B,
		YEN = 0x7D,
		ABNT_C2 = 0x7E,
		NUMPADEQUALS = 0x8D,
		PREVTRACK = 0x90,
		AT = 0x91,
		COLON = 0x92,
		UNDERLINE = 0x93,
		KANJI = 0x94,
		STOP = 0x95,
		AX = 0x96,
		UNLABELED = 0x97,
		NEXTTRACK = 0x99,
		NUMPADENTER = 0x9C,
		RCONTROL = 0x9D,
		MUTE = 0xA0,
		CALCULATOR = 0xA1,
		PLAYPAUSE = 0xA2,
		MEDIASTOP = 0xA4,
		VOLUMEDOWN = 0xAE,
		VOLUMEUP = 0xB0,
		WEBHOME = 0xB2,
		NUMPADCOMMA = 0xB3,
		NUMPADSLASH = 0xB5,
		SYSRQ = 0xB7,
		RALT = 0xB8,
		PAUSE = 0xC5,
		HOME = 0xC7,
		UP = 0xC8,
		PGUP = 0xC9,
		LEFT = 0xCB,
		RIGHT = 0xCD,
		END = 0xCF,
		DOWN = 0xD0,
		PGDN = 0xD1,
		INSERT = 0xD2,
		DELETEKEY = 0xD3,
		LWIN = 0xDB,
		RWIN = 0xDC,
		APPS = 0xDD,
		POWER = 0xDE,
		SLEEP = 0xDF,
		WAKE = 0xE3,
		WEBSEARCH = 0xE5,
		WEBFAVORITES = 0xE6,
		WEBREFRESH = 0xE7,
		WEBSTOP = 0xE8,
		WEBFORWARD = 0xE9,
		WEBBACK = 0xEA,
		MYCOMPUTER = 0xEB,
		MAIL = 0xEC,
		MEDIASELECT = 0xED,
		KEY_CODE_MAX = 0xEE
	};

	// �p�b�h�{�^���R�[�h
	enum PadButton{
		A = 0,
		B = 1,
		X = 2,
		Y = 3,
		L = 4,
		R = 5,
		START = 6,
		BACK = 7,
		LS = 8,
		RS = 9,
		BUTTON_MAX = 10
	};

	//------------------------------------------------------------------------------
	// �N���X���@�FController
	// �N���X�T�v�F���͏����X�V,�i�[����N���X
	//------------------------------------------------------------------------------
	class Controller{
	public:
		// �{�^���̍ő吔
		static const int BUTTON_MAX = 10;

		Controller();
		~Controller(){ UnInit(); }

		// ������
		void Init(unsigned int JoystickID = 1U, bool bKeyboard = false, bool bVibration = true);
		
		// �I������
		void UnInit();
		
		// ���͏��̍X�V
		void Update();
		
		// �h�����Ă���W���C�X�e�B�b�N�̐��̎擾
		int GetJoystickMax()const;
		
		// �֘A�t������Ă���W���C�X�e�B�b�N�̔ԍ��̎擾
		inline unsigned int GetJoystickID()const{ return joystickID_; }
		
		// ���͂̃N���A
		void ZeroClear();
		
		// ���L�[�̓��͕����̎擾(���K���ς�)
		Vector2D GetArrow();
		
		// ��L�[���̓t���[�����̎擾
		inline unsigned int GetUp()const{ return up_; }
		
		// ���L�[���̓t���[�����̎擾
		inline unsigned int GetDown()const{ return down_; }
		
		// ���L�[���̓t���[�����̎擾
		inline unsigned int GetLeft()const{ return left_; }
		
		// �E�L�[���̓t���[�����̎擾
		inline unsigned int GetRight()const{ return right_; }
		
		// ���A�i���O�X�e�B�b�N�̓��͕����̎擾(���K���ς�)
		inline Vector2D GetLAxis()const{ return Vector2D(lAxisX_, lAxisY_).Normalized(); }
		
		// ���A�i���O�X�e�B�b�NX���̎擾
		inline float GetLAxisX()const{ return lAxisX_; }
		
		// ���A�i���O�X�e�B�b�NY���̎擾
		inline float GetLAxisY()const{ return lAxisY_; }
		
		// �E�A�i���O�X�e�B�b�N�̓��͕����̎擾(���K���ς�)
		inline Vector2D GetRAxis()const{ return Vector2D(rAxisX_, rAxisY_).Normalized(); }
		
		// �E�A�i���O�X�e�B�b�NX���̎擾
		inline float GetRAxisX()const{ return rAxisX_; }
		
		// �E�A�i���O�X�e�B�b�NY���̎擾
		inline float GetRAxisY()const{ return rAxisY_; }
		
		// ���g���K�[�̎擾
		inline float GetLTrigger()const{ return lTrigger_; }
		
		// �E�g���K�[�̎擾
		inline float GetRTrigger()const{ return rTrigger_; }

		// �{�^���̓��̓t���[�����̎擾
		unsigned int GetButton(unsigned int buttonID)const;
		
		// �w�肵���L�[��������Ă��邩
		bool IsKeyPress(enum KeyCode keyCode)const;
		
		// �U���̊J�n
		bool SetVibration(unsigned int time, float leftSpeed = 1.0f, float rightSpeed = 1.0f);
		
		// �w�肵���W���C�X�e�B�b�N�{�^�����{�^���ԍ��Ɋ֘A�t����
		void SetConfig(unsigned int configID, unsigned int buttonID);
		
		// �w�肵���L�[���{�^���ԍ��Ɋ֘A�t����
		void SetConfig(unsigned int configID, enum KeyCode keyCode);

		// �{�^���ԍ��Ɋ֘A�t����ꂽ�W���C�X�e�B�b�N�{�^���̔ԍ����擾����
		int GetConfig(unsigned int configID)const;

		// ���v���C�f�[�^�L�^�J�n
		bool RecReplayData();

		// ���v���C�f�[�^�Đ��J�n
		bool PlayReplayData(bool bCueing = true);

		// ���v���C�f�[�^�̒�~
		bool StopReplayData();
		
		// ���v���C�f�[�^�̏���
		bool ClearReplayData();

		// ���v���C�f�[�^�̕ۑ�
		bool SaveReplayData(const char* pFileName);
		
		// ���v���C�f�[�^�̓ǂݍ���
		bool LoadReplayData(const char* pFileName);

		// ���v���C���^�撆��
		bool IsRecordingReplayData()const;

		// ���v���C���Đ�����
		bool IsPlayingReplayData()const;

	protected:
		// �W���C�X�e�B�b�N�ԍ�
		unsigned int joystickID_ = 0U;

		// �L�[�{�[�h�̎g�p�t���O
		bool bKeyboard_ = false;

		unsigned int up_ = 0U;		// ��L�[�̓��̓t���[����
		unsigned int down_ = 0U;		// ���L�[�̓��̓t���[����
		unsigned int left_ = 0U;		// ���L�[�̓��̓t���[����
		unsigned int right_ = 0U;	// �E�L�[�̓��̓t���[����

		float lAxisX_ = 0.0f;			// ���A�i���O�X�e�B�b�N��X��
		float lAxisY_ = 0.0f;			// ���A�i���O�X�e�B�b�N��Y��

		float rAxisX_ = 0.0f;			// �E�A�i���O�X�e�B�b�N��X��
		float rAxisY_ = 0.0f;			// �E�A�i���O�X�e�B�b�N��Y��

		float lTrigger_ = 0.0f;		// ���g���K�[
		float rTrigger_ = 0.0f;		// �E�g���K�[

		// �{�^���̓��̓t���[����
		unsigned int button_[BUTTON_MAX] = {};

		// �U���̎g�p�t���O
		bool bVibration_ = false;

		// �U������
		unsigned int vibrationTime_ = 0U;

	private:
#pragma pack(push,1)
		// ���̓��O
		struct InputLog{
			short keyData;	// �{�^���̓���
			short lAxisX;	// ���A�i���O�X�e�B�b�N��X��
			short lAxisY;	// ���A�i���O�X�e�B�b�N��Y��
			short rAxisX;	// �E�A�i���O�X�e�B�b�N��X��
			short rAxisY;	// �E�A�i���O�X�e�B�b�N��Y��
			short lTrigger;	// ���g���K�[
			short rTrigger;	// �E�g���K�[
		};
#pragma pack(pop)

		// ���v���C���[�h
		enum class ReplayMode{
			REPLAY_NONE,
			REPLAY_REC,
			REPLAY_PLAY,
			REPLAY_STOP,
			REPLAY_MAX
		};

		// ���v���C�f�[�^
		std::vector<struct InputLog> replayLog_;
		
		// ���v���C�f�[�^�̃C�e���[�^
		std::vector<struct InputLog>::iterator itReplayLog_;
		
		// �R���t�B�O(�W���C�X�e�B�b�N�{�^��)
		unsigned int config_[BUTTON_MAX] = {};

		// �R���t�B�O(�L�[�{�[�h)
		enum KeyCode keyConfig_[BUTTON_MAX] = {};
		
		// ���v���C���[�h
		enum ReplayMode rMode_ = ReplayMode::REPLAY_NONE;
	};
}

#endif	// #ifndef INCLUDE_IDEA_CONTROLLER_H