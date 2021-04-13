#ifndef INCLUDE_IDEA_FILELOADER_H
#define INCLUDE_IDEA_FILELOADER_H

#include "IFileLoader.h"

namespace idea{
	//------------------------------------------------------------------------------
	// �N���X���@�FFileLoader
	// �N���X�T�v�F�t�@�C������ǂݍ��ރN���X
	//------------------------------------------------------------------------------
	class FileLoader : public IFileLoader
	{
	public:
		// �t�@�C�����̓ǂݍ���
		virtual bool Load(std::vector<BYTE>& out, const char* filename, DWORD offset = 0, DWORD size = 0)const;
	};
}

#endif	// #ifndef INCLUDE_IDEA_FILELOADER_H