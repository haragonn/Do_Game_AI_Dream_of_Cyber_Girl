#include "../../h/Archive/VirtualFileLoader.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

using namespace std;
using namespace idea;

//------------------------------------------------------------------------------
// コンストラクタ
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
// ファイルを読み込む
//------------------------------------------------------------------------------
bool VirtualFileLoader::Load(std::vector<BYTE>& out, const char* filename, DWORD offset, DWORD size)const
{
	if(NULL == filename){ return false; }

	std::string name;

	if(::PathIsRelative(filename)){
		// 相対パス
		name = GetPath();
		name += filename;

		auto it = vecLoader_.rbegin();

		while(vecLoader_.rend() != it){
			const LoaderPair& pair = (*it);

			if(pair.loader->Load(out, (pair.path + name).c_str(), offset, size)){ return true; }

			++it;
		}
	} else{
		// 絶対パス
		name = filename;

		// 絶対パスの場合,デフォルトパスを無視する
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
// 新しいファイルローダを登録する
//------------------------------------------------------------------------------
bool VirtualFileLoader::PushLoader(SFileLoader loader, const char* path)
{
	if(NULL == loader || NULL == path){ return false; }

	vecLoader_.push_back(LoaderPair(loader, path));

	return true;
}

//------------------------------------------------------------------------------
// 最後に登録されたファイルローダをスタックから下ろす
//------------------------------------------------------------------------------
bool VirtualFileLoader::PopLoader()
{
	if(vecLoader_.empty()){ return false; }

	vecLoader_.pop_back();

	return true;
}
