#ifndef INCLUDE_IDEA_VIRTUALFILELOADER_H
#define INCLUDE_IDEA_VIRTUALFILELOADER_H

#include "IFileLoader.h"
#include "PathManager.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace idea{
	//------------------------------------------------------------------------------
	// �N���X���@�FVirtualFileLoader
	// �N���X�T�v�F�����t�H���_�ɎU���Ă���t�@�C����,1�̃t�H���_���ɑ��݂��邩��
	// �@�@�@�@�@�@�悤�ɉ��z�I�Ɉ�����t�@�C�����[�_,�p�X�Ǘ��ƃ��[�h�@�\������
	//------------------------------------------------------------------------------
	class VirtualFileLoader : public IFileLoader, public PathManager{
	public:
		// �V�����t�@�C�����[�_��o�^����
		virtual bool PushLoader(SFileLoader loader, const char* path);

		// �Ō�ɓo�^���ꂽ�t�@�C�����[�_���X�^�b�N���牺�낷
		virtual bool PopLoader();

		// �t�@�C����ǂݍ���
		virtual bool Load(std::vector<BYTE>& out, const char* filename, DWORD offset = 0, DWORD size = 0)const;

		// ���[�_�[���
		struct LoaderPair
		{
			LoaderPair(SFileLoader loader, std::string path);

			// ���[�_�[
			SFileLoader loader;

			// ��_�p�X��
			std::string	path;
		};

		// ���[�_�[�̃��X�g
		std::vector<LoaderPair> vecLoader_;
	};
}

#endif	// INCLUDE_IDEA_VIRTUALFILELOADER_H
