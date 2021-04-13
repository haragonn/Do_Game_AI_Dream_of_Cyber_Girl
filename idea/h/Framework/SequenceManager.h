/*==============================================================================
	[SequenceManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_SEQUENCEMANAGER_H
#define INCLUDE_IDEA_SEQUENCEMANAGER_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <atomic>

namespace idea{
	class Scene;

	//------------------------------------------------------------------------------
	// �N���X���@�FSequenceManager
	// �N���X�T�v�F�V���O���g��
	// �@�@�@�@�@�@�V�[�P���X������s���N���X
	//------------------------------------------------------------------------------
	class SequenceManager{
	public:
		// �B��̃C���X�^���X��Ԃ�
		static SequenceManager& Instance()
		{
			static SequenceManager s_Instance;
			return s_Instance;
		}

		// ������
		bool Init(Scene* pInitScene);
		
		// �I������
		void UnInit();

		// �X�V
		int Update(bool bRedy);

		// �`��
		void Draw();

		// �V�[���̎擾
		Scene* GetScenePtr(){ return pScene_; }

		// ���̃V�[�����Z�b�g
		void SetNextScenePtr(Scene* pNextScene){ pNextScene_ = pNextScene; }

	private:
		friend class FrameworkManager;

		// ���݂̃V�[��
		Scene* pScene_ = nullptr;

		// ���̃V�[��
		Scene* pNextScene_ = nullptr;

		// �񓯊��ǂݍ��݃X���b�h�̃n���h��
		HANDLE hAsyncLoadThread_ = NULL;

		// �񓯊��ǂݍ��݂̊����t���O
		std::atomic<bool> bLoadCompleted_;

		// �N���e�B�J���Z�N�V����
		CRITICAL_SECTION cs_ = {};

		// �񓯊��ǂݍ��݃X���b�h
		friend unsigned __stdcall AsyncLoadThread(void* vp);

		// �񓯊��ǂݍ���
		inline void AsyncLoad();

		SequenceManager();
		~SequenceManager();

		// �R�s�[�R���X�g���N�^�̋֎~
		SequenceManager(const SequenceManager& src){}
		SequenceManager& operator=(const SequenceManager& src){}
	};
}

#endif	// #ifndef INCLUDE_IDEA_SEQUENCEMANAGER_H