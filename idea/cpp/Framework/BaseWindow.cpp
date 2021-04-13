/*==============================================================================
	[BaseWindow.cpp]
														Author	:	Keigo Hara
==============================================================================*/
#include "../../h/Framework/BaseWindow.h"

#include <stdarg.h>
#include <vector>

using namespace idea;

//------------------------------------------------------------------------------
// �R���X�g���N�^
//------------------------------------------------------------------------------
BaseWindow::BaseWindow():
	hWnd_(0),
	msg_(0),
	wp_(0),
	lp_(0)
{
	ZeroMemory(&wcex_, sizeof(WNDCLASSEX));
	ZeroMemory(title_, sizeof(char) * 256);
}

//------------------------------------------------------------------------------
// �E�B���h�E�^�C�g���̕ύX
//------------------------------------------------------------------------------
bool BaseWindow::SetWindowFormatText(const char* pFormat, ...)
{
	va_list args;	// �ό��̈������X�g

	// �t�H�[�}�b�g�w�肳�ꂽ��������o�͂���
	va_start(args, pFormat);
	int len = _vscprintf(pFormat, args) + 1;
	std::vector<char> strBuffer;
	strBuffer.resize(len);
	vsprintf_s(&strBuffer[0], len, pFormat, args);
	va_end(args);

	strcpy_s(title_, 255, &strBuffer[0]);

	return !!SetWindowText(hWnd_, &strBuffer[0]);	// ��������^�C�g���ɔ��f������
}

//------------------------------------------------------------------------------
// ���b�Z�[�W�̊Ď�
//------------------------------------------------------------------------------
bool BaseWindow::ProcessMessage()
{
	MSG msg;

	if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)){	// ���b�Z�[�W�͗��Ă��Ȃ���
		if(!GetMessage(&msg, NULL, 0, 0)){ return false; }	// �I���̃��b�Z�[�W�����Ă����false��Ԃ�
		TranslateMessage(&msg);	// ���b�Z�[�W��Ԃ�
		DispatchMessage(&msg);
	}

	return true;
}

//------------------------------------------------------------------------------
// �E�B���h�E�v���V�[�W��
//------------------------------------------------------------------------------
LRESULT CALLBACK BaseWindow::WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{

#ifdef _WIN64
	BaseWindow* pThis = (BaseWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);	// �E�B���h�E�Ɋ֘A�t�����Ă���f�[�^�̎擾
#else
	BaseWindow* pThis = (BaseWindow*)GetWindowLong(hWnd, GWL_USERDATA);	// �E�B���h�E�Ɋ֘A�t�����Ă���f�[�^�̎擾
#endif
	if(!pThis){
		if(msg == WM_NCCREATE && lp){ pThis = (BaseWindow*)((LPCREATESTRUCT)lp)->lpCreateParams; }	// �E�B���h�E�����ꂽ��

#ifdef _WIN64
		if(pThis){ SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis); }	// lp�Ɋi�[���ꂽCREATESTRUCT�\���̂ւ̃|�C���^�[��n��
#else
		if(pThis){ SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)pThis); }	// lp�Ɋi�[���ꂽCREATESTRUCT�\���̂ւ̃|�C���^�[��n��
#endif
	}

	if(pThis){
		// �E�B���h�E�n���h���⃁�b�Z�[�W�����i�[����
		pThis->hWnd_ = hWnd;
		pThis->msg_ = msg;
		pThis->wp_ = wp;
		pThis->lp_ = lp;
		return pThis->MessageProc();
	}

	return DefWindowProc(hWnd, msg, wp, lp);
}

//------------------------------------------------------------------------------
// ���b�Z�[�W�̊Ǘ�
//------------------------------------------------------------------------------
LRESULT BaseWindow::MessageProc()
{
	// ���b�Z�[�W����
	switch(msg_){
	case WM_CREATE:
		return OnCreate();
	case WM_CLOSE:
		return OnClose();
	case WM_DESTROY:
		return OnDestroy();
	case WM_ACTIVATE:
		return OnActivate();
	case WM_SETFOCUS:
		return OnSetFocus();
	case WM_KILLFOCUS:
		return OnKillFocus();
	case WM_MOVE:
		return OnMove();
	case WM_SIZE:
		return OnSize();
	case WM_PAINT:
		return OnPaint();
	case WM_KEYDOWN:
		return OnKeyDown();
	case WM_SYSKEYDOWN:
		return OnSysKeyDown();
	case WM_SETCURSOR:
		return OnSetCursor();
	}

	return DefaultProc();
}