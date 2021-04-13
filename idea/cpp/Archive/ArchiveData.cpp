#include "../../h/Archive/ArchiveData.h"
#include "../../h/Archive/FileLoader.h"
#include <vector>
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

#define _CRT_DISABLE_PERFCRIT_LOCKS

using namespace std;
using namespace idea;

//------------------------------------------------------------------------------
// ���̂̐���
//------------------------------------------------------------------------------
ArchiveData* ArchiveData::CreateInstance(const char* archiveFileName)
{
	string name;

	name = archiveFileName;

	vector<byte> buffer;

	// �w�b�_�ǂݍ���
	FileLoader loader;

	if(!loader.Load(buffer, archiveFileName, 0, 8)){
		return NULL;
	}

	// �}�W�b�N�i���o�[���`�F�b�N
	if('\0' != buffer[0] || 'I' != buffer[1] || 'A' != buffer[2] || 'F' != buffer[3]){
		return NULL;
	}

	uint32_t headerSize = *(uint32_t*)&(buffer[4]);

	// �w�b�_�[������,�}�W�b�N�i���o�[�ƃw�b�_�[�T�C�Y�̈��������������ǂݍ���
	if(!loader.Load(buffer, archiveFileName, 8, headerSize - 8)){
		return NULL;
	}

	// ���
	uint32_t fileNum = *(uint32_t*)&(buffer[0]);
	vector<string> filePaths;
	vector<uint32_t> fileSizes;
	filePaths.reserve(fileNum);
	fileSizes.reserve(fileNum);

	DWORD i = 4;
	for(uint32_t n = 0; n < fileNum; ++n){
		uint32_t length = *(uint32_t*)&(buffer[i]);
		i += sizeof(uint32_t);

		string fileName = string((const char*)&buffer[i], 0, length / sizeof(char));
		filePaths.push_back(fileName);
		i += length;

		fileSizes.push_back(*(uint32_t*)&buffer[i]);
		i += sizeof(uint32_t);
	}

	// �t�@�C���w�b�_�̃��X�g���쐬����
	FileHeaderList headers;

	// �t�@�C���擪�ւ̃I�t�Z�b�g�l
	// �����l�̓w�b�_�[�T�C�Y��
	DWORD offset = headerSize;

	for(uint32_t n = 0; n < fileNum; ++n){
		FileHeader h;
		h.dwIndex = n;
		h.dwFileOffset = offset;
		h.dwFileSize = fileSizes[n];

		headers.insert(pair<string, FileHeader>(filePaths[n], h));

		offset += fileSizes[n];
	}

	ArchiveData* p = new ArchiveData;
	p->archiveFileName_ = name;
	p->mapHeader_ = headers;

	return p;
}

//------------------------------------------------------------------------------
// �f�[�^�̓ǂݍ���
//------------------------------------------------------------------------------
bool ArchiveData::Load(vector<byte>& out, const char* filename, DWORD offset, DWORD size)const
{
	vector<byte>().swap(out);
	string name = filename;

	// �p�X���̐��K��
	{
		int n = 0;
		while(-1 != (n = (int)name.find('/', n))){
			name = name.replace(n, 1, "\\");
		}

		vector<char> p(name.size() + 1);

		if(!::PathCanonicalize(&p[0], name.c_str())){ return false; }

		name = string(&p[0]);
	}

	// ��΃p�X�ł̓��[�h�ł��Ȃ�
	if(!::PathIsRelative(name.c_str())){ return false; }

	// �t�@�C�����Ō���
	FileHeaderList::const_iterator it = mapHeader_.find(name);
	if(mapHeader_.end() == it){ return false; }

	// �����Ƀq�b�g�����t�@�C���̃w�b�_�����Q��
	const FileHeader& header = it->second;

	// �I�t�Z�b�g���t�@�C���T�C�Y�ȏ�Ȃ�G���[
	if(header.dwFileSize < offset){ return false; }

	if(0 == size){
		size = (header.dwFileSize - offset);
	} else if((header.dwFileSize - offset) < size){
		size = (header.dwFileSize - offset);
	}

	vector<byte> buffer;

	if(0 < header.dwFileSize){
		FileLoader loader;

		if(!loader.Load(buffer, archiveFileName_.c_str(), header.dwFileOffset + offset, size)){ return false; }
	} else{
		buffer.resize(0);
	}

	// ����Ƀf�[�^���擾�ł����̂�,swap���g��out�Ɣz��Q�Ƃ�����
	out.swap(buffer);

	return true;
}
