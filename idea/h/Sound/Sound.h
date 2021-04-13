/*==============================================================================
	[Sound.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLIDE_IDEA_SOUND_H
#define INCLIDE_IDEA_SOUND_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct IXAudio2SourceVoice;

namespace idea{
	//------------------------------------------------------------------------------
	// �N���X���@�FSound
	// �N���X�T�v�Fwave�t�@�C���̓ǂݍ���,�\�[�X�{�C�X�̊Ǘ�,�Đ����s��
	//------------------------------------------------------------------------------
	class Sound{
	public:
		Sound();
		~Sound();

		// wave�t�@�C������T�E���h��ǂݍ���
		bool LoadWaveFromFile(const char* pFileName);

		// �A�[�J�C�u�t�@�C���̒���wave�t�@�C������T�E���h��ǂݍ���
		bool LoadWaveFromArchiveFile(const char* pArchiveFileName, const char* pFileName);

		// �X�g���[�W����T�E���h��ǂݍ���
		bool LoadWaveFromStorage(const char* pFileName);

		// �\�[�X�{�C�X�̔j��
		void UnLoad();

		// ���y�̍Đ�
		void Play(bool bLoop = false, bool bCueing = true, UINT32 begin = 0);

		// ���y�̒�~
		void Stop();

		// ���y�̓��o��
		void Cueing();

		// ���ʂ̐ݒ�
		void SetVolume(float volume = 1.0f);

		// �Đ�����
		bool IsPlaying();

		// ���[�v���Ă��邩
		bool IsLooping();

	private:
		friend class SoundManager;

		// �t�@�C����
		char fileName_[256] = {};

		// �\�[�X�{�C�X
		IXAudio2SourceVoice* pSourceVoice_ = nullptr;

		// �I�[�f�B�I���
		BYTE* pDataAudio_ = nullptr;

		// �I�[�f�B�I�T�C�Y
		DWORD audioSize_ = 0U;

		// �Đ�����
		bool bPlay_ = false;

		// ���[�v���Ă��邩
		bool bLoop_ = false;

		// �X�g���[�W�g�p�t���O
		bool bStorage_ = false;

		// �o�^�t���O
		bool bRegistered_ = false;

		// �O�̃m�[�h
		Sound* pPrev_ = nullptr;

		// ���̃m�[�h
		Sound* pNext_ = nullptr;

		// �R�s�[�R���X�g���N�^�̋֎~
		Sound(const Sound& src){}
		Sound& operator=(const Sound& src){}
	};
}

#endif // #ifndef INCLIDE_IDEA_SOUND_H