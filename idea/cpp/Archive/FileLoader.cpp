#include "../../h/Archive/FileLoader.h"
#include <shlwapi.h>
#include <string>

#pragma comment(lib, "shlwapi.lib")

#define _CRT_DISABLE_PERFCRIT_LOCKS

using namespace std;
using namespace idea;

//------------------------------------------------------------------------------
// �t�@�C�����̓ǂݍ���
//------------------------------------------------------------------------------
bool FileLoader::Load(vector<byte>& out, const char* filename, DWORD offset, DWORD size)const
{
	vector<byte>().swap(out);

	std::string name = filename;

	// �p�X���̐��K��
	{
		int n = 0;

		while(-1 != (n = (int)name.find('/', n))){
			name = name.replace(n, 1, "\\");
		}

		std::vector<char> p(name.size() + 1);

		if(!::PathCanonicalize(&p[0], name.c_str())){ return false; }

		name = std::string(&p[0]);
	}

	FILE* fp = NULL;
	if(0 != fopen_s(&fp, name.c_str(), "rb")){ return false; }

	fseek(fp, 0, SEEK_END);

	DWORD fileSize = ftell(fp);

	if(fileSize < offset){
		fclose(fp);
		return false;
	}

	if(0 == size){
		size = (fileSize - offset);
	} else if((fileSize - offset) < size){
		size = (fileSize - offset);
	}

	fseek(fp, offset, SEEK_SET);

	// �ǂݍ��݃o�b�t�@���m��
	std::vector<byte> buffer(size);

	if(0 < size){
		DWORD readSize = (DWORD)fread(&buffer[0], 1, size, fp);
		if(readSize != size){
			fclose(fp);
			return false;
		}
	}

	fclose(fp);

	// ����Ƀf�[�^���擾�ł����̂ŁAswap���g��out�Ɣz��Q�Ƃ�����
	out.swap(buffer);

	return true;
}

