/*==============================================================================
	[SequenceManager.cpp]
														Author	:	Keigo Hara
==============================================================================*/
#include <exception>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>

#include "../../h/Framework/SequenceManager.h"
#include "../../h/Framework/Scene.h"
#include "../../h/Framework/AsyncLoadScene.h"
#include "../../h/2D/Actor2DManager.h"
#include "../../h/3D/Actor3DManager.h"
#include "../../h/Storage/StorageManager.h"
#include "../../h/Network/NetworkManager.h"
#include "../../h/Utility/ideaUtility.h"

using namespace idea;

namespace idea{
	//------------------------------------------------------------------------------
	// �񓯊��ǂݍ��݃X���b�h
	//------------------------------------------------------------------------------
	unsigned __stdcall AsyncLoadThread(void* vp)
	{
		SequenceManager* psq = (SequenceManager*)vp;

		EnterCriticalSection(&psq->cs_);

		psq->AsyncLoad();
		psq->bLoadCompleted_ = true;

		LeaveCriticalSection(&psq->cs_);

		_endthreadex(0);

		return 0U;
	}
}

//------------------------------------------------------------------------------
// �R���X�g���N�^
//------------------------------------------------------------------------------
SequenceManager::SequenceManager() :
	pScene_(nullptr),
	pNextScene_(nullptr),
	hAsyncLoadThread_(nullptr),
	bLoadCompleted_(false){}

//------------------------------------------------------------------------------
// �f�X�g���N�^
//------------------------------------------------------------------------------
SequenceManager::~SequenceManager()
{
	UnInit();
}

//------------------------------------------------------------------------------
// ������
//------------------------------------------------------------------------------
bool SequenceManager::Init(Scene* pInitScene)
{
	if(!pInitScene){	// NULL�`�F�b�N
		return false;
	}

	if(!pScene_){	// ���ɃV�[���������Ă��Ȃ����
		pNextScene_ = pScene_ = pInitScene;	// �V�[����o�^

		StorageManager::Instance().CreatNullInstance();

		pScene_->Init();

		return true;
	}

	SafeDelete(pInitScene);	// �V�[���̔j��

	return false;
}

//------------------------------------------------------------------------------
// �I������
//------------------------------------------------------------------------------
void SequenceManager::UnInit()
{
	// �X���b�h�̔j��
	if(hAsyncLoadThread_){
#ifdef _DEBUG
		// �f�o�b�O�Ȃ�2����������ŎE�����Ⴆ
		if(WaitForSingleObject(hAsyncLoadThread_, 2 * 60 * 1000) == WAIT_TIMEOUT){
			MessageBox(NULL, "AsyncLoadThread���I�����܂���B�����I�����܂��B", NULL, NULL);
			TerminateThread(hAsyncLoadThread_, FALSE);
		}
#else
		WaitForSingleObject(hAsyncLoadThread_, INFINITE);
#endif
		CloseHandle(hAsyncLoadThread_);
		hAsyncLoadThread_ = nullptr;
	}

	// �V�[���̔j��
	if(pScene_){
		pScene_->UnInit();

		StorageManager::Instance().ReleaseNullInstance();

		SafeDelete(pScene_);
	}
}

//------------------------------------------------------------------------------
// �X�V
//------------------------------------------------------------------------------
int SequenceManager::Update(bool bRedy)
{
	if(pScene_){	// NULL�`�F�b�N
		if(pScene_->bAsync_){	// �񓯊��ǂݍ��݃V�[���Ȃ�
			if(!bLoadCompleted_ && !hAsyncLoadThread_){
				// �N���e�B�J���Z�N�V����������
				InitializeCriticalSection(&cs_);
				hAsyncLoadThread_ = (HANDLE)_beginthreadex(NULL, 0, AsyncLoadThread, this, 0, NULL);	// �񓯊��ǂݍ��݃X���b�h�̍쐬
			} else{
				// �񓯊��ǂݍ��݃X���b�h�̊Ď�
				DWORD ExitCode;
				GetExitCodeThread(hAsyncLoadThread_, &ExitCode);
				if(ExitCode != STILL_ACTIVE){
					CloseHandle(hAsyncLoadThread_);
					hAsyncLoadThread_ = nullptr;
				}
			}
		} else{
			bLoadCompleted_ = true;
		}

		pScene_->Update();	// �V�[���̃A�b�v�f�[�g���玟�̃V�[�����󂯎��

		Actor2DManager::Instance().UpdatePrePosition();
		Actor3DManager::Instance().UpdatePrePosition();

		NetworkManager::Instance().WritingEnd();	// �l�b�g���[�N��������

		if(pNextScene_ == pScene_){	// �V�[���ς��Ȃ���΂��̂܂ܑ��s
			return 0;
		}

		// �񓯊��ǂݍ��݃X���b�h�̔j��
		if(hAsyncLoadThread_){
			if(WaitForSingleObject(hAsyncLoadThread_, INFINITE) == WAIT_TIMEOUT){ throw std::exception(); }
			CloseHandle(hAsyncLoadThread_);
			hAsyncLoadThread_ = nullptr;
		}

		// �قȂ�V�[�����󂯎������O�̃V�[����j�����ăV�[����؂�ւ���
		pScene_->UnInit();

		NetworkManager::Instance().WritingEnd();

		SafeDelete(pScene_);

		// �X�g���[�W�̔j���v���`�F�b�N
		auto& sm = StorageManager::Instance();

		if(sm.IsUnLoadAllRequest()){
			// �X�g���[�W��S�Ĕj��
			sm.UnLoadAll();
		} else{
			// �摜��S�Ĕj��
			if(sm.GetUnLoadImageRequestSize()){
				sm.UnLoadImage();
			}

			// �T�E���h��S�Ĕj��
			if(sm.GetUnLoadWaveRequestSize()){
				sm.UnLoadWave();
			}

			// OBJ���b�V���̔j��
			if(sm.GetUnLoadObjModelRequestSize()){
				sm.UnLoadObjModel();
			}

			// PMX���b�V���̔j��
			if(sm.GetUnLoadPmxModelRequestSize()){
				sm.UnLoadPmxModel();
			}

			// PMX���b�V���̔j��
			if(sm.GetUnLoadVmdMotionRequestSize()){
				sm.UnLoadVmdMotion();
			}
		}

		// �V�[���J��
		if(pNextScene_ && bRedy){	// ���̃V�[���������
			pScene_ = pNextScene_;

			if(pScene_->bAsync_){
				bLoadCompleted_ = false;
			}

			pScene_->Init();

			return 1;	// �V�[����ύX���đ��s
		}
	}

	StorageManager::Instance().ReleaseNullInstance();

	return -1;	// ���f
}

//------------------------------------------------------------------------------
// �`��
//------------------------------------------------------------------------------
void SequenceManager::Draw()
{
	if(pScene_){	// NULL�`�F�b�N
		pScene_->Draw();
	}
}

//------------------------------------------------------------------------------
// �񓯊��ǂݍ���
//------------------------------------------------------------------------------
void SequenceManager::AsyncLoad()
{
	if(!pScene_ || !pScene_->bAsync_){	// NULL�`�F�b�N,�񓯊��t���O�`�F�b�N
		return;
	}

	if(dynamic_cast<AsyncLoadScene*>(pScene_)->bLoadCompleted_){
		return;
	}

	dynamic_cast<AsyncLoadScene*>(pScene_)->Load();

	dynamic_cast<AsyncLoadScene*>(pScene_)->bLoadCompleted_ = true;
}