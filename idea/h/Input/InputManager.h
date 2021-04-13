/*==============================================================================
	[InputManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_INPUTMANAGER_H
#define INCLUDE_IDEA_INPUTMANAGER_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

namespace idea{
	//------------------------------------------------------------------------------
	// �\���̒�`
	//------------------------------------------------------------------------------
#pragma pack(push,1)

	struct MouseInput{
		// �{�^���̍ő吔
		static const int BUTTON_MAX = 4;

		// X���W
		int posX;

		// �O�t���[����X���W
		int prePosX;

		// X���W�̍���
		int differenceX;

		// Y���W
		int posY;

		// �O�t���[����Y���W
		int prePosY;

		// Y���W�̍���
		int differenceY;

		// �{�^���̉����ꂽ�t���[����
		unsigned int button[BUTTON_MAX];

		// �z�C�[���̍���
		int wheel;
	};

	struct Joystick{
		// �{�^���̍ő吔
		static const int BUTTON_MAX = 10;

		// �{�^���������ꂽ���ǂ���
		bool button[BUTTON_MAX];

		// ��L�[�������ꂽ���ǂ���
		bool up;

		// ���L�[�������ꂽ���ǂ���
		bool down;

		// ���L�[�������ꂽ���ǂ���
		bool left;

		// �E�L�[�������ꂽ���ǂ���
		bool right;

		// ���A�i���O�X�e�B�b�N��X��
		float lAxisX;

		// ���A�i���O�X�e�B�b�N��Y��
		float lAxisY;

		// �E�A�i���O�X�e�B�b�N��X��
		float rAxisX;

		// �E�A�i���O�X�e�B�b�N��Y��
		float rAxisY;

		// ���g���K�[
		float lTrigger;

		// �E�g���K�[
		float rTrigger;
	};

#pragma pack(pop)

	//------------------------------------------------------------------------------
	// �N���X���@�FInputManager
	// �N���X�T�v�F�V���O���g��
	// �@�@�@�@�@�@DirectInput�f�o�C�X�̊Ǘ����s��
	// �@�@�@�@�@�@�L�[�{�[�h,�}�E�X,�W���C�X�e�B�b�N�̓��͏����X�V,�Ǘ�����
	//------------------------------------------------------------------------------
	class InputManager{
	public:
		static const int JOYSTIC_MAX = 4;	// �W���C�X�e�B�b�N�̍ő吔

		// �B��̃C���X�^���X��Ԃ�
		static InputManager& Instance()
		{
			static InputManager s_Instance;
			return s_Instance;
		}

		//========================================
		// DirectInput
		//========================================
		// ������
		bool Init(HINSTANCE hInst, HWND hWnd);

		// �I������
		void UnInit();

		// �S�Ă̓��͏��̍X�V
		bool UpdateAll();

		//========================================
		// �L�[�{�[�h
		//========================================
		// �L�[�{�[�h�̓��͏��̍X�V
		inline bool UpdateKeyState();

		// �w�肵���L�[��������Ă��邩
		bool IsKeyPress(const DWORD keyCode){ return !!(keyState_[keyCode] & 0x80); }

		//========================================
		// �}�E�X
		//========================================
		// �}�E�X�̓��͏��̍X�V
		inline bool UpdateMouseState();

		// �}�E�X�̓��͏��̎擾
		const MouseInput& GetMouse()const{ return mouseState_; }

		//========================================
		// �W���C�X�e�B�b�N
		//========================================
		// �W���C�X�e�B�b�N�̓��͏��̍X�V
		inline bool UpdateJoystick();

		// �h�����Ă���W���C�X�e�B�b�N�̐��̎擾
		int GetJoystickNum()const{ return joystickNum_; }

		// �W���C�X�e�B�b�N��U��������
		bool SetVibration(DWORD joystickID, float leftSpeed, float rightSpeed);

		// �W���C�X�e�B�b�N�̓��͏��̎擾
		const Joystick& GetJoystick(DWORD JoystickID);

	private:
		// �W���C�X�e�B�b�N�񋓂̃R�[���o�b�N�֐�
		static BOOL CALLBACK EnumJoysticksCallback(LPCDIDEVICEINSTANCE pInst, LPVOID pvRef);

		// �A�i���O�X�e�B�b�N�̃R�[���o�b�N�֐�
		static BOOL CALLBACK EnumAxesCallback(LPCDIDEVICEOBJECTINSTANCE pInst, LPVOID pvRef);

		// �C���X�^���X
		HINSTANCE hInst_ = NULL;

		// �E�B���h�E�n���h��
		HWND hWnd_ = NULL;

		// �C���^�[�t�F�C�X
		LPDIRECTINPUT8 pDInput_ = nullptr;

		// �f�o�C�X�̔\��
		DIDEVCAPS devCaps_ = {};

		// �L�[�{�[�h�̃f�o�C�X
		LPDIRECTINPUTDEVICE8 pKeyboardDev_ = nullptr;

		// �L�[�{�[�h�̓��͏��
		BYTE keyState_[256] = {};

		// �}�E�X�̃f�o�C�X
		LPDIRECTINPUTDEVICE8 pMouseDev_ = nullptr;

		// �}�E�X�̓��͏��
		MouseInput mouseState_;

		// �W���C�X�e�B�b�N���g���Ă��Ȃ������ꍇ�̋�̏��
		static Joystick s_NullJoystick_;

		// �W���C�X�e�B�b�N�̃f�o�C�X
		LPDIRECTINPUTDEVICE8 pJoystickDev_[JOYSTIC_MAX] = {};

		// �h�����Ă�W���C�X�e�B�b�N�̐�
		DWORD joystickNum_ = 0U;

		// �W���C�X�e�B�b�N�f�o�C�X�̐�
		DWORD joystickDevNum_ = 0U;

		// �W���C�X�e�B�b�N�̍ő吔
		DWORD maxJoystickNum_ = 0U;

		// �W���C�X�e�B�b�N�̓��͏��
		Joystick* pJoystick_ = nullptr;

		InputManager();
		~InputManager(){ UnInit(); }

		// �R�s�[�R���X�g���N�^�̋֎~
		InputManager(const InputManager& src){}
		InputManager& operator=(const InputManager& src){}
	};
}

#endif // #ifndef INCLUDE_IDEA_INPUTMANAGER_H