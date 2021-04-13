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
	// ���C�����[�v�X���b�h
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
// �R���X�g���N�^
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
// ������
//------------------------------------------------------------------------------
bool FrameworkManager::Init(HINSTANCE hInst, int nCmdShow, UINT width, UINT height, bool bWindowed, UINT fps, UINT frameSkipMax, bool bNetwork, bool bDebug, const char* pClassName)
{
	SetLeakCheckFlag();	// ���������[�N�`�F�b�N�̃t���O���I����

	if(bReady_){	// �������ς݂Ȃ玸�s
		return false;
	}

	// �����𔽉f
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
	// �E�B���h�E
	//========================================
	// �E�B���h�E�N���X�̓o�^
	wcex_.cbSize = sizeof(WNDCLASSEX);						// �\���̃T�C�Y
	wcex_.style = CS_HREDRAW | CS_VREDRAW;					// �E�B���h�E�N���X�X�^�C��
	wcex_.lpfnWndProc = (WNDPROC)WndProc;					// �E�B���h�E�v���V�[�W��
	wcex_.cbClsExtra = 0;									// �⏕�̈�T�C�Y
	wcex_.cbWndExtra = 0;									// �⏕�̈�T�C�Y
	wcex_.hInstance = hInst_;								// �C���X�^���X�n���h��
	wcex_.hIcon = LoadIcon(NULL, IDI_APPLICATION);			// �A�C�R��
	wcex_.hCursor = LoadCursor(NULL, IDC_ARROW);			// �}�E�X�J�[�\��
	wcex_.hbrBackground = (HBRUSH)(COLOR_WINDOWTEXT + 1);	// �N���C�A���g�̈�w�i�F
	wcex_.lpszMenuName = NULL;								// �E�B���h�E���j���[
	wcex_.lpszClassName = pClassName;						// �E�B���h�E�N���X��
	wcex_.hIconSm = LoadIcon(NULL, IDI_APPLICATION);		// ���A�C�R��
	if(!RegisterClassEx(&wcex_)){ return false; }

	// �E�B���h�E�̍쐬
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
		MessageBox(hWnd_, "DirectX11���������ł��܂���ł���", NULL, NULL);
		return false;
	}

	if(!SpriteManager::Instance().Init()){
		MessageBox(hWnd_, "2D�V�F�[�_�[���������ł��܂���ł���", NULL, NULL);
		return false;
	}

	if(!SpriteInstancingManager::Instance().Init()){
		MessageBox(hWnd_, "2D�V�F�[�_�[���������ł��܂���ł���(2)", NULL, NULL);
		return false;
	}

	if(!ObjectManager::Instance().Init()){
		MessageBox(hWnd_, "3D�V�F�[�_�[���������ł��܂���ł���", NULL, NULL);
		return false;
	}

	if(!InputManager::Instance().Init(hInst_, hWnd_)){
		MessageBox(hWnd_, "DirectInput���������ł��܂���ł���", NULL, NULL);
		return false;
	}

	if(!SoundManager::Instance().Init()){
		MessageBox(hWnd_, "XAudio2���������ł��܂���ł���", NULL, NULL);
		return false;
	}

	//========================================
	// �t�F�[�h
	//========================================
	FadeManager::Instance().Init((float)width_, (float)height_);

	//========================================
	// Winsock
	//========================================
	if(bNetwork){
		if(!NetworkManager::Instance().Init()){
			MessageBox(hWnd_, "Winsock���������ł��܂���ł���", NULL, NULL);
			return false;
		}
	}

	//========================================
	// ImGui
	//========================================
	if(!ImGuiManager::Instance().Init()){
		MessageBox(hWnd_, "ImGui���������ł��܂���ł���", NULL, NULL);
		return false;
	}

	if(bDebug){
		DebugManager::Instance().Init();
	}

	// �E�B���h�E�̕\��
	if(!bWindowed){
		bChangeDispRequest_ = true;
	}
	ValidateRect(hWnd_, 0);			// WM_PAINT���Ă΂�Ȃ��悤�ɂ���
	UpdateWindow(hWnd_);			// �N���C�A���g�̈�̍X�V
	ShowWindow(hWnd_, nCmdShow);	// �\����Ԃ̐ݒ�

	bStorage_ = true;				// �X�g���[�W�̎g�p�\�t���O���I����
	bReady_ = true;					// �����t���O���I����

	return true;
}

//------------------------------------------------------------------------------
// �I������
//------------------------------------------------------------------------------
void FrameworkManager::UnInit()
{
	if(!bReady_){	// ���������Ă��Ȃ���Ύ��s
		return;
	}

	bReady_ = false;	// �����t���O���I�t��

	if(hMainLoopThread_){
		// ���C�����[�v�X���b�h�̏I����҂�
#ifdef _DEBUG
		// �f�o�b�O�Ȃ�2����������E�����Ⴆ
		if(WaitForSingleObject(hMainLoopThread_, 2 * 60 * 1000) == WAIT_TIMEOUT){
			MessageBox(hWnd_, "MainLoopThread���I�����܂���B�����I�����܂��B", NULL, NULL);
			TerminateThread(hMainLoopThread_, FALSE);
		}
#else
		WaitForSingleObject(hMainLoopThread_, INFINITE);
#endif
		CloseHandle(hMainLoopThread_);
		hMainLoopThread_ = nullptr;
	}

	// �e�}�l�[�W���̏I������
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
// ���C�����[�v
//------------------------------------------------------------------------------
void FrameworkManager::MainLoop()
{
	SequenceManager& sq = SequenceManager::Instance();	// �V�[�P���X�̃C���X�^���X
	GraphicManager& gm = GraphicManager::Instance();	// �O���t�B�b�N�}�l�[�W���̃C���X�^���X
	FadeManager& fm = FadeManager::Instance();			// �t�F�[�h�̃C���X�^���X
	ImGuiManager& im = ImGuiManager::Instance();		// ImGui�}�l�[�W���̃C���X�^���X
	DebugManager& dm = DebugManager::Instance();		// �f�o�b�O�}�l�[�W���̃C���X�^���X

	if(!sq.Init(pInitScene_)){	// �V�[�P���X�̏�����
		sq.UnInit();
		return;
	}

	Timer tmr;				// �^�C�}�[
	Timer tmrFps;			// FPS�v���p�^�C�}�[
	Timer tmrUpdate;
	double frameTime = 1000.0 / targetFps_;	// 1�t���[���̒P�ʎ���
	unsigned long long frameCount = 0U;		// �t���[����
	UINT fpsCount = 0U;		// FPS�J�E���g

	tmr.SetStartTime();		// �^�C�}�[�J�n
	tmrFps.SetStartTime();	//

	while(bReady_){
		if(bChangeDispRequest_){	// �f�B�X�v���C���[�h�؂�ւ����N�G�X�g�󂯎��
			bChangeDispRequest_ = false;
			bChangeDispReady_ = false;
		}

		if(!bChangeDispReady_){		// �f�B�X�v���C���[�h��؂�ւ��Ă���Œ��Ȃ璆�~
			frameCount = 1U;
			fpsCount = 0U;
			frameSkipCount_ = 0U;

			tmr.SetStartTime();
			tmrFps.SetStartTime();

			continue;
		}

		// FPS�̌v��
		if(fpsCount == targetFps_){
			fps_ = (float)(targetFps_ * 1000.0 / tmrFps.GetHighPrecisionElapsed());

			fpsCount = 0U;

			tmrFps.SetStartTime();
		}

		double elapsed = tmr.GetHighPrecisionElapsed();

		if(elapsed > frameTime * (frameCount + 1) || NetworkManager::Instance().GetSkipFlag()){	// �ڕW�̎��Ԃ���������
			tmrUpdate.SetStartTime();

			im.Update();	// imGui�̍X�V

			InputManager::Instance().UpdateAll();	// ����

			int sqTemp = sq.Update(bReady_);	// �X�V

			// �I���v��
			if(sqTemp == -1){
				break;
			}

			fm.Update();	// �t�F�[�h�̍X�V

			if(bDebug_){
				dm.Update(sq.bLoadCompleted_);	// �f�o�b�O���̍X�V
			}

			updateTime_ = tmrUpdate.GetHighPrecisionElapsed();

			// �`��
			if(!(frameSkipMax_ > frameSkipCount_ && updateTime_ > frameTime || NetworkManager::Instance().GetSkipFlag())){

				if(gm.BeginScene()){
					gm.EndMask();	// �}�X�N�̃N���[��

					sq.Draw();		// �V�[�P���X�̕`��

					fm.Draw();		// �t�F�[�h�̕`��

					im.Draw();		// ImGui�̕`��

					gm.EndScene();

					frameSkipCount_ = 0U;
				}
			} else{
				im.Draw();		// ImGui�̕`��

				++frameSkipCount_;
			}

			if(sqTemp == 1){	// �V�[���̐؂�ւ����t���[���J�E���g������������
				frameCount = 0U;
				fpsCount = 0U;
				frameSkipCount_ = 0U;

				// �f�o�b�O���̍X�V
				if(bDebug_){
					dm.RefreshTime();
				}

				// ���Ԍv���J�n
				tmr.SetStartTime();
				tmrFps.SetStartTime();

				continue;
			}

			// �t���[���J�E���g�̍X�V
			if(elapsed < frameTime * (frameCount + 2)){
				++frameCount;
			} else{
				frameCount = static_cast<unsigned long long>(elapsed / frameTime);
			}

			++fpsCount;
		} else{
			NetworkManager::Instance().WritingEnd();
			Sleep(1);	// �ڕW�̎��Ԃ������Ă��Ȃ���΋x��
		}

		if(NetworkManager::Instance().GetSkipFlag()){
			NetworkManager::Instance().WritingEnd();
			Sleep(1);
		}
	}

	sq.UnInit();
}

//------------------------------------------------------------------------------
// �V�[���̎��s
//------------------------------------------------------------------------------
void FrameworkManager::Run(Scene* pInitScene)
{
	if(bExit_){	// Run()�̍ČĂяo���̏���
		SafeDelete(pInitScene);
		return;
	}

	if(!bReady_ || !pInitScene){	// �����͂�����,�V�[����NULL�`�F�b�N
		return;
	}

	pInitScene_ = pInitScene;

	DWORD ExitCode;

	// �N���e�B�J���Z�N�V����������
	InitializeCriticalSection(&cs_);

	// ���C�����[�v�͕ʃX���b�h�ɓ�����
	hMainLoopThread_ = (HANDLE)_beginthreadex(NULL, 0, MainLoopThread, this, 0, NULL);

	// ���C���X���b�h�ł̓��b�Z�[�W������,���C�����[�v�X���b�h,�f�o�C�X���Ď�����
	while(ProcessMessage()){
		// �f�B�X�v���C���[�h�̐؂�ւ�
		if(!bChangeDispReady_){
			ChangeDisplayMode();
		}

		ImGuiManager::Instance().SetWndProcHandler(hWnd_, msg_, wp_, lp_);	// ImGui�̃v���V�[�W��

		// �Ȉ�FPS�\��
		if(bFpsShow_){
			char temp[16] = {};
			sprintf_s(temp, " fps[%2.1f]", static_cast<int>(fps_ * 10) * 0.1f);
			char buf[300] = {};
			strcpy_s(buf, 300, title_);
			strcat_s(buf, 300, temp);
			SetWindowText(hWnd_, buf);
		}

		// ���C�����[�v�X���b�h�ƃE�B���h�E�̊Ď�
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
// �}�E�X�J�[�\���̉����̐ݒ�
//------------------------------------------------------------------------------
void FrameworkManager::SetMouseCursorVisibleFlag(bool bOnWindowed, bool bOnFullscreen)
{
	// �t���O�̔��f
	bOnWindowed_ = bOnWindowed;
	bOnFullscreen_ = bOnFullscreen;
}

//------------------------------------------------------------------------------
// �E�B���h�E���[�h�̐؂�ւ�
//------------------------------------------------------------------------------
void FrameworkManager::ChangeDisplayMode()
{
	bWindowed_ = !bWindowed_;	// �E�B���h�E���[�h�t���O��؂�ւ���

	GraphicManager::Instance().ChangeDisplayMode(bWindowed_);	// �O���t�B�b�N�}�l�[�W���ɃE�B���h�E���[�h�؂�ւ���v��

	// �E�B���h�E�̕ύX
	if(bWindowed_){	// �E�C���h�E��
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
	} else{	// �t���X�N���[����
		RECT dr;
		GetWindowRect(GetDesktopWindow(), &dr);

		int dw = dr.right - dr.left;
		int dh = dr.bottom - dr.top;

		SetWindowLong(hWnd_, GWL_STYLE, FULL_SCREEN_STYLE);
		SetWindowPos(hWnd_, HWND_TOPMOST, 0, 0, dw, dh, SWP_SHOWWINDOW);
	}

	// ���N�G�X�g����
	bChangeDispRequest_ = false;
	bChangeDispReady_ = true;
}

//------------------------------------------------------------------------------
// �E�B���h�E������ꂽ��
//------------------------------------------------------------------------------
LRESULT FrameworkManager::OnClose()
{
	DestroyWindow(hWnd_);

	return 0;
}

//------------------------------------------------------------------------------
// �E�B���h�E���j������鎞
//------------------------------------------------------------------------------
LRESULT FrameworkManager::OnDestroy()
{
	UnInit();

	PostQuitMessage(0);	// �I�����b�Z�[�W�𑗂�

	return 0;
}

//------------------------------------------------------------------------------
// �L�[�����͂��ꂽ��
//------------------------------------------------------------------------------
LRESULT FrameworkManager::OnKeyDown(){
	if(wp_ == VK_ESCAPE){	// �G�X�P�[�v�L�[�������ꂽ��
		if(bWindowed_){	// �E�B���h�E��
#ifdef _DEBUG
			// �f�o�b�O���͂����Ȃ�I��
			SendMessage(hWnd_, WM_CLOSE, 0, 0);	// �I��
			return 0;
#endif
			// �I���m�F�𑣂�
			char temp[256] = {};
			sprintf_s(temp, "�w%s�x���I�����܂����H", wcex_.lpszClassName);
			if(MessageBox(hWnd_, temp, "�I���m�F", MB_OKCANCEL) == IDOK){
				SendMessage(hWnd_, WM_CLOSE, 0, 0);	// �I��
			}
		} else if(!bChangeDispRequest_){	// �t���X�N���[����

			bChangeDispRequest_ = true;	// �E�B���h�E���[�h�̐؂�ւ�
		}

		return 0;
	} else{
		return DefaultProc();
	}
}

//------------------------------------------------------------------------------
// �V�X�e���L�[�����͂��ꂽ��
//------------------------------------------------------------------------------
LRESULT FrameworkManager::OnSysKeyDown()
{
	// Alt+�G���^�[�L�[�������ꂽ��E�B���h�E���[�h�̐؂�ւ�
	if(wp_ != VK_RETURN){
		return DefaultProc();
	}

	if(!NetworkManager::Instance().IsConnect() && !bChangeDispRequest_){
		bChangeDispRequest_ = true;
	}

	return 0;
}

//------------------------------------------------------------------------------
// �E�B���h�E���Ń}�E�X�J�[�\������������
//------------------------------------------------------------------------------
LRESULT FrameworkManager::OnSetCursor()
{
	// �t���O�ɍ��킹�ĕ\����؂�ւ���
	if(bWindowed_ ? !bOnWindowed_ : !bOnFullscreen_){
		SetCursor(NULL);	// �}�E�X���\��

		return 0;
	} else{
		return DefaultProc();
	}
}