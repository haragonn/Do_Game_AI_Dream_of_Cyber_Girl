#ifndef INCLUDE_IDEA_PATHMANAGER_H
#define INCLUDE_IDEA_PATHMANAGER_H

#include "IPathManager.h"
#include <vector>
#include <string>

namespace idea{
	//------------------------------------------------------------------------------
	// �N���X���@�FPathManager
	// �N���X�T�v�F�t�@�C���p�X���Ǘ�����N���X
	//------------------------------------------------------------------------------
	class PathManager : public IPathManager
	{
	public:
		// �X�^�b�N�Ƀp�X��o�^����
		virtual bool PushPath(const char* path);

		
		// �X�^�b�N����ŐV�̓o�^�p�X����菜��
		virtual bool PopPath();

		
		// ���݂̃J�����g�p�X���擾����
		virtual const char* GetPath()const;

	private:
		// �p�X�̃��X�g
		std::vector<std::string> vecPath_;
	};
}

#endif	// #ifndef INCLUDE_IDEA_PATHMANAGER_H