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
	// �N���X���@�FFramework
	// �N���X�T�v�F�E�B���h�E�̊Ǘ�,�Q�[���̃��C�����[�v���s��
	//------------------------------------------------------------------------------
	class Framework{
	public:

		// ������
		static bool Init(HINSTANCE hInst, int nCmdShow, const FrameworkInitDesc& fid);

		static bool Init(HINSTANCE hInst, int nCmdShow, UINT width, UINT height, bool bWindowed = true, UINT fps = 60U, UINT frameSkipMax = 0U, bool bNetwork = false, bool bDebug = false, const char* pClassName = "idea");

		// �I������
		static void UnInit();

		// �V�[���̎��s
		static void Run(Scene* pInitScene);

		// �E�B���h�E�^�C�g���̕ύX
		static bool SetWindowFormatText(const char* pFormat, ...);

		// FPS�̎擾
		static float GetFps();

		// �ڕWFPS�̎擾
		static UINT GetTargetFps();

		// FPS�ȈՕ\���̐ݒ�
		static void SetFpsSimpleDisplayFlag(bool bShow);

		// �X�V�����ɂ����������Ԃ̎擾
		static double GetUpdateTime();

		// �}�E�X�J�[�\���̉����̐ݒ�
		static void SetMouseCursorVisibleFlag(bool bOnWindowed, bool bOnFullscreen);
	};
};

#endif	// #ifndef INCLUDE_IDEA_FRAMEWORK_H