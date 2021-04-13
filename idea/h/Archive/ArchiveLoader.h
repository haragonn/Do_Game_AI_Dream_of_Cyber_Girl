/*==============================================================================
	[ArchiveLoader.cpp]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_ARCHIVELOADER_H
#define INCLUDE_IDEA_ARCHIVELOADER_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vector>

namespace idea{
	//------------------------------------------------------------------------------
	// �N���X���@�FArchiveLoader
	// �N���X�T�v�F�A�[�J�C�u�t�@�C���̓ǂݍ���,�Ǘ����s���N���X
	//------------------------------------------------------------------------------
	class ArchiveLoader{
	public:
		ArchiveLoader(){}
		~ArchiveLoader(){ UnLoad(); }

		// �A�[�J�C�u�t�@�C���̓ǂݍ���
		bool Load(const char* archiveFileName, const char* fileName);

		// �f�[�^�̎擾
		std::vector<BYTE>& GetData(){ return data_; }

		// �f�[�^�̃T�C�Y�̎擾
		size_t GetSize(){ return data_.size(); }

		// �f�[�^�̔j��
		void UnLoad();

	private:
		// �f�[�^
		std::vector<BYTE> data_;

		// �R�s�[�R���X�g���N�^�̋֎~
		ArchiveLoader(const ArchiveLoader& src){}
		ArchiveLoader& operator=(const ArchiveLoader& src){}
	};
}

#endif	// #ifndef INCLUDE_IDEA_ARCHIVELOADER_H