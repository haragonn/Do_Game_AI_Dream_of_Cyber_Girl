/*==============================================================================
	[SoundManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_SOUNDMANAGER_H
#define INCLUDE_IDEA_SOUNDMANAGER_H

#define WIN32_LEAN_AND_MEAN
#include <XAudio2.h>


namespace idea{
	class Sound;

	//------------------------------------------------------------------------------
	// �N���X���@�FSoundManager
	// �N���X�T�v�F�V���O���g��
	// �@�@�@�@�@�@XAudio2�I�u�W�F�N�g�ւ̃C���^�[�t�F�C�X�ƃ}�X�^�[�{�C�X�̊Ǘ����s��
	//------------------------------------------------------------------------------
	class SoundManager{
	public:
		// �B��̃C���X�^���X��Ԃ�
		static SoundManager& Instance()
		{
			static SoundManager s_Instance;
			return s_Instance;
		}

		// ������
		bool Init();
		
		// �I������
		void UnInit();

		// �\�[�X�{�C�X�̐���
		bool CreateSourceVoice(IXAudio2SourceVoice** ppSourceVoice, WAVEFORMATEX* pSourceFormat);

		// �T�E���h�̓o�^
		void Register(Sound* pSound);

		// �T�E���h�̓o�^����
		void UnRegister(Sound* pSound);

	private:
		// XAudio2�I�u�W�F�N�g�ւ̃C���^�[�t�F�C�X
		IXAudio2* pXAudio2_ = nullptr;

		// �}�X�^�[�{�C�X
		IXAudio2MasteringVoice* pMasteringVoice_ = nullptr;

		// CoInitializeEx()�����t���O
		bool bCom_ = false;

		// ���X�g�̎n�܂�
		Sound* pListBegin_ = nullptr;
		
		// ���X�g�̏I���
		Sound* pListEnd_ = nullptr;

		SoundManager();
		~SoundManager(){ UnInit(); }

		// �R�s�[�R���X�g���N�^�̋֎~
		SoundManager(const SoundManager& src){}
		SoundManager& operator=(const SoundManager& src){}
	};
}

#endif // #ifndef INCLUDE_IDEA_SOUNDMANAGER_H