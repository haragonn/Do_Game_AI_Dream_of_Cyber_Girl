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
	// �N���X���@�FBaseWindow
	// �N���X�T�v�F�E�B���h�E������Window�A�v���P�[�V�����̊��ƂȂ�N���X
	// �@�@�@�@�@�@��Ƀ��b�Z�[�W�Ǘ����s��
	//------------------------------------------------------------------------------
	class BaseWindow{
	public:
		BaseWindow();
		virtual ~BaseWindow(){}
		
		// �E�B���h�E�^�C�g���̕ύX
		bool SetWindowFormatText(const char* pFormat, ...);

	protected:
		// �E�B���h�E�n���h��
		HWND hWnd_ = NULL;
		
		// �E�B���h�E�N���X
		WNDCLASSEX wcex_ = {};
		
		// ���b�Z�[�W
		UINT msg_ = 0U;
		
		// �p�����[�^
		WPARAM wp_ = NULL;
		
		// �p�����[�^
		LPARAM lp_ = NULL;
		
		// �E�C���h�E�^�C�g��
		char title_[256] = {};

		// ���b�Z�[�W�̊Ď�
		bool ProcessMessage();

		// �E�B���h�E�v���V�[�W��
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
		
		// ���b�Z�[�W�̊Ǘ�
		virtual inline LRESULT MessageProc();
		
		// �E�B���h�E���������ꂽ��
		virtual LRESULT OnCreate()		{ return DefaultProc(); }
		
		// �E�B���h�E������ꂽ��
		virtual LRESULT OnClose()		{ return DefaultProc(); }
		
		// �E�B���h�E���j������鎞
		virtual LRESULT OnDestroy()		{ PostQuitMessage(0); return 0; }
		
		// �E�B���h�E�̃A�N�e�B�u��,��A�N�e�B�u���̒ʒm
		virtual LRESULT OnActivate()	{ return DefaultProc(); }
		
		// �L�[�{�[�h�t�H�[�J�X�𓾂���
		virtual LRESULT OnSetFocus()	{ return DefaultProc(); }
		
		// �L�[�{�[�h�t�H�[�J�X����������
		virtual LRESULT OnKillFocus()	{ return DefaultProc(); }
		
		// �E�B���h�E���ړ����ꂽ��
		virtual LRESULT OnMove()		{ return DefaultProc(); }
		
		// �E�B���h�E�̃T�C�Y���ύX���ꂽ��
		virtual LRESULT OnSize()		{ return DefaultProc(); }
		
		// �`�悷�鎞
		virtual LRESULT OnPaint()		{ return DefaultProc(); }
		
		// �L�[�����͂��ꂽ��
		virtual LRESULT OnKeyDown()		{ return DefaultProc(); }
		
		// �V�X�e���L�[�����͂��ꂽ��
		virtual LRESULT OnSysKeyDown()	{ return DefaultProc(); }
		
		// �E�B���h�E���Ń}�E�X�J�[�\������������
		virtual LRESULT OnSetCursor()	{ return DefaultProc(); }
		
		// �f�t�H���g�̏���
		inline LRESULT DefaultProc()	{ return DefWindowProc(hWnd_, msg_, wp_, lp_); }
	};
}

#endif // #ifndef INCLUDE_IDEA_BASEWINDOW_H