#ifndef INCLUDE_IDEA_IPATHMANAGER_H
#define INCLUDE_IDEA_IPATHMANAGER_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <memory>

namespace idea{
	//------------------------------------------------------------------------------
	// �N���X���@�FIPathManager
	// �N���X�T�v�F�t�@�C���p�X�}�l�[�W���̊��N���X
	//------------------------------------------------------------------------------
	class IPathManager
	{
	public:
		virtual ~IPathManager(){}

		// �X�^�b�N�Ƀp�X��o�^����
		virtual bool PushPath(const char* path) = 0;


		// �X�^�b�N����ŐV�̓o�^�p�X����菜��
		virtual bool PopPath() = 0;


		// ���݂̃J�����g�p�X���擾����
		virtual const char* GetPath()const = 0;


		// �p�X�𐳋K������
		static bool NormalizePath(std::string& path);
	};

	// IPathManager��shared_ptr
	typedef std::shared_ptr<IPathManager> SPathManager;
}

#endif	// #ifndef INCLUDE_IDEA_IPATHMANAGER_H