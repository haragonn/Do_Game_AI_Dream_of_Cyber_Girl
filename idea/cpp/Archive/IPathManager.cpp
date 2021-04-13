#include "../../h/Archive/IPathManager.h"
#include <shlwapi.h>
#include <vector>

#pragma comment(lib, "shlwapi.lib")

using namespace idea;

//------------------------------------------------------------------------------
// �p�X�𐳋K������
//------------------------------------------------------------------------------
bool IPathManager::NormalizePath(std::string& path)
{
	// �p�X���R�s�[
	std::string tmp = path;

	// �S�Ă�'/'��,'\\'�ɒu�������邱�Ƃ�,�p�X�L�@�ɓ��ꐫ����������
	int n = 0;
	while(-1 != (n = (int)tmp.find('/', n))){
		tmp = tmp.replace(n, 1, "\\");
	}

	// �s�v��"."��".."�Ȃǂ̕������܂ރp�X����,�����̕������܂܂Ȃ��p�X���ɕϊ�����
	std::vector<char> p(tmp.size() + 1);

	if(!::PathCanonicalize(&p[0], tmp.c_str())){
		return false;
	}

	path = std::string(&p[0]);

	return true;
}