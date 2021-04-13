#ifndef INCLUDE_IDEA_IFILELOADER_H
#define INCLUDE_IDEA_IFILELOADER_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vector>
#include <memory>

namespace idea{
	//------------------------------------------------------------------------------
	// �N���X���@�FIFileLoader
	// �N���X�T�v�F�t�@�C�����[�_�[�̊��N���X
	//------------------------------------------------------------------------------
	class IFileLoader
	{
	public:
		virtual ~IFileLoader(){};

		// �t�@�C�������[�h����
		virtual bool Load(std::vector<BYTE>& out, const char* filename, DWORD offset = 0U, DWORD size = 0U)const = 0;
	};

	// IFileLoader��shared_ptr
	typedef std::shared_ptr<IFileLoader> SFileLoader;
}

#endif	// #ifndef INCLUDE_IDEA_IFILELOADER_H