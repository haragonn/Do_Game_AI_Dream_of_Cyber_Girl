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
	// �N���X���@�FFrameworkManager
	// �N���X�T�v�F�V���O���g��
	// �@�@�@�@�@�@�E�B���h�E�̊Ǘ�,�Q�[���̃��C�����[�v���s��
	//------------------------------------------------------------------------------
	class FrameworkManager : public BaseWindow{
	public:
		// �B��̃C���X�^���X��Ԃ�
		static FrameworkManager& Instance()
		{
			static FrameworkManager s_Instance;
			return s_Instance;
		}

		// ������
		bool Init(HINSTANCE hInst, int nCmdShow, UINT width, UINT height, bool bWindowed = true, UINT fps = 60U, UINT frameSkipMax = 0U, bool bNetwork = false, bool bDebug = false, const char* pClassName = "idea");

		// �I������
		void UnInit();

		// �V�[���̎��s
		void Run(Scene* pInitScene);

		// FPS�̎擾
		float GetFps(){ return fps_; }

		// �ڕWFPS�̎擾
		UINT GetTargetFps()const{ return targetFps_; }

		// FPS�ȈՕ\���̐ݒ�
		void SetFpsSimpleDisplayFlag(bool bShow){ bFpsShow_ = bShow; }

		// �X�V�����ɂ����������Ԃ̎擾
		double GetUpdateTime()const{ return updateTime_; }

		// �}�E�X�J�[�\���̉����̐ݒ�
		void SetMouseCursorVisibleFlag(bool bOnWindowed, bool bOnFullscreen);

	private:
		// �C���X�^���X�n���h��
		HINSTANCE hInst_ = NULL;

		// �X�N���[���̕�
		UINT width_ = 0U;

		// �X�N���[���̍���
		UINT height_ = 0U;

		// �E�B���h�E���[�h
		bool bWindowed_ = false;

		// ���s��������
		std::atomic<bool> bReady_;

		// �����V�[��
		std::atomic<Scene*> pInitScene_;

		// �I���t���O
		bool bExit_ = false;

		// �E�B���h�E���̃}�E�X�J�[�\���\���ݒ�
		bool bOnWindowed_ = false;

		// �t���X�N���[�����̃}�E�X�J�[�\���\���ݒ�
		bool bOnFullscreen_ = false;

		// �E�B���h�E���[�h�̐؂�ւ��v��
		std::atomic<bool> bChangeDispRequest_;

		// �E�C���h�E���[�h�̐؂�ւ��̊���
		std::atomic<bool> bChangeDispReady_;

		// �ڕW��FPS
		std::atomic<UINT> targetFps_;

		// ���݂�FPS
		float fps_ = 0.0f;

		// FPS�̊ȈՕ\���t���O
		bool bFpsShow_ = false;

		// �X�V�����ɂ�����������
		std::atomic<double> updateTime_;

		// �ő�t���[���X�L�b�v��
		std::atomic<UINT> frameSkipMax_;

		// �t���[���X�L�b�v��
		std::atomic<UINT> frameSkipCount_;

		// �X�g���[�W�̎g�p�\�t���O
		bool bStorage_ = false;

		// �f�o�b�O�@�\�g�p�t���O
		std::atomic<bool> bDebug_;

		// ���C�����[�v�X���b�h�̃n���h��
		HANDLE hMainLoopThread_ = NULL;

		// �N���e�B�J���Z�N�V����
		CRITICAL_SECTION cs_ = {};

		// ���C�����[�v�X���b�h
		friend unsigned __stdcall MainLoopThread(void* vp);

		// ���C�����[�v
		inline void MainLoop();

		// �E�B���h�E���[�h�̐؂�ւ�
		inline void ChangeDisplayMode();

		// �E�B���h�E������ꂽ��
		LRESULT OnClose()override;

		// �E�B���h�E���j������鎞
		LRESULT OnDestroy()override;

		// �L�[�����͂��ꂽ��
		LRESULT OnKeyDown()override;

		// �V�X�e���L�[�����͂��ꂽ��
		LRESULT OnSysKeyDown()override;

		// �E�B���h�E���Ń}�E�X�J�[�\������������
		LRESULT OnSetCursor()override;

		FrameworkManager();
		~FrameworkManager(){}

		// �R�s�[�R���X�g���N�^�̋֎~
		FrameworkManager(FrameworkManager& src){}
		FrameworkManager& operator=(FrameworkManager& src){}
	};
}

#endif // #ifndef INCLUDE_IDEA_FLAMEWORKMANAGER_H