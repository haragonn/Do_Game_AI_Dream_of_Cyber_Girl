#include "../../h/Archive/PathManager.h"
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

using namespace idea;

//------------------------------------------------------------------------------
// �X�^�b�N�Ƀp�X��o�^����
//------------------------------------------------------------------------------
bool PathManager::PushPath(const char* pszPath)
{
	std::string path;

	if(::PathIsRelative(pszPath)){
		// ���΃p�X�̏ꍇ�A�J�����g�p�X�ƘA������
		path = GetPath();
		path += pszPath;
	} else{
		// ��΃p�X�̏ꍇ�́A�J�����g�p�X�͖�������
		path = pszPath;
	}

	// �p�X�̐��K�����s��
	if(!NormalizePath(path)){ return false; }

	// �����ɂ�'\\'���K������悤�ɂ��Ă���
	if(!path.empty() && '\\' != path[path.size() - 1]){
		path += '\\';
	}

	// �X�^�b�N�Ƀp�X��o�^
	vecPath_.push_back(path);

	return true;
}

//------------------------------------------------------------------------------
// �X�^�b�N����ŐV�̓o�^�p�X����菜��
//------------------------------------------------------------------------------
bool PathManager::PopPath()
{
	if(vecPath_.empty()){ return false; }

	vecPath_.pop_back();

	return true;
}

//------------------------------------------------------------------------------
// ���݂̃J�����g�p�X���擾����
//------------------------------------------------------------------------------
const char* PathManager::GetPath()const
{
	const static char tmp[] = "";

	if(vecPath_.empty()){ return tmp; }

	return vecPath_.back().c_str();
}
