#ifndef INCLUDE_IDEA_ARCHIVEDATA_H
#define INCLUDE_IDEA_ARCHIVEDATA_H

#pragma warning (disable:4786)

#include "IFileLoader.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <unordered_map>

namespace idea{
	//------------------------------------------------------------------------------
	// �N���X���@�FArchiveData
	// �N���X�T�v�F�A�[�J�C�u�t�@�C����̃f�[�^���Ǘ�����N���X
	//------------------------------------------------------------------------------
	class ArchiveData : public IFileLoader{
	public:
		// ���̂̐���
		static ArchiveData* CreateInstance(const char* archiveFileName);

		// �f�[�^�̓ǂݍ���
		virtual bool Load(std::vector<BYTE>& out, const char* filename, DWORD offset = 0, DWORD size = 0)const;

	protected:
		// �A�[�J�C�u�t�@�C���̃w�b�_�[
		typedef struct{
			// �t�@�C���ԍ�
			DWORD dwIndex;

			// �t�@�C���擪�I�t�Z�b�g�l
			DWORD dwFileOffset;

			// �t�@�C���T�C�Y 
			DWORD dwFileSize;
		}FileHeader;

		// �t�@�C���̃w�b�_�[���X�g
		typedef std::unordered_map<std::string, FileHeader> FileHeaderList;

		// �}�E���g�����A�[�J�C�u�t�@�C���l�[��
		std::string	archiveFileName_;

		// �t�@�C���̃w�b�_�[���X�g
		FileHeaderList mapHeader_;

		ArchiveData(){}
	};
}

#endif	// #ifndef INCLUDE_IDEA_ARCHIVEDATA_H