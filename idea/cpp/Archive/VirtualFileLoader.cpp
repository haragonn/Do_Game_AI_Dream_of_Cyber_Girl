#include "../../h/Archive/VirtualFileLoader.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

using namespace std;
using namespace idea;

//------------------------------------------------------------------------------
// �R���X�g���N�^
//------------------------------------------------------------------------------
VirtualFileLoader::LoaderPair::LoaderPair(SFileLoader _loader, std::string _path) :
	path(_path),
	loader(_loader)
{
	if(!path.empty() && '/' != path[path.size() - 1] && '\\' != path[path.size() - 1]){
		path += '/';
	}
}

//------------------------------------------------------------------------------
// �t�@�C����ǂݍ���
//------------------------------------------------------------------------------
bool VirtualFileLoader::Load(std::vector<BYTE>& out, const char* filename, DWORD offset, DWORD size)const
{
	if(NULL == filename){ return false; }

	std::string name;

	if(::PathIsRelative(filename)){
		// ���΃p�X
		name = GetPath();
		name += filename;

		auto it = vecLoader_.rbegin();

		while(vecLoader_.rend() != it){
			const LoaderPair& pair = (*it);

			if(pair.loader->Load(out, (pair.path + name).c_str(), offset, size)){ return true; }

			++it;
		}
	} else{
		// ��΃p�X
		name = filename;

		// ��΃p�X�̏ꍇ,�f�t�H���g�p�X�𖳎�����
		auto it = vecLoader_.rbegin();

		while(vecLoader_.rend() != it){
			const LoaderPair& pair = (*it);

			if(pair.loader->Load(out, name.c_str(), offset, size)){ return true; }

			++it;
		}
	}

	return false;
}

//------------------------------------------------------------------------------
// �V�����t�@�C�����[�_��o�^����
//------------------------------------------------------------------------------
bool VirtualFileLoader::PushLoader(SFileLoader loader, const char* path)
{
	if(NULL == loader || NULL == path){ return false; }

	vecLoader_.push_back(LoaderPair(loader, path));

	return true;
}

//------------------------------------------------------------------------------
// �Ō�ɓo�^���ꂽ�t�@�C�����[�_���X�^�b�N���牺�낷
//------------------------------------------------------------------------------
bool VirtualFileLoader::PopLoader()
{
	if(vecLoader_.empty()){ return false; }

	vecLoader_.pop_back();

	return true;
}
