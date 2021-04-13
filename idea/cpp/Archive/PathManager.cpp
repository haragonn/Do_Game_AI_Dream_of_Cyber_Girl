#include "../../h/Archive/PathManager.h"
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

using namespace idea;

//------------------------------------------------------------------------------
// スタックにパスを登録する
//------------------------------------------------------------------------------
bool PathManager::PushPath(const char* pszPath)
{
	std::string path;

	if(::PathIsRelative(pszPath)){
		// 相対パスの場合、カレントパスと連結する
		path = GetPath();
		path += pszPath;
	} else{
		// 絶対パスの場合は、カレントパスは無視する
		path = pszPath;
	}

	// パスの正規化を行う
	if(!NormalizePath(path)){ return false; }

	// 末尾には'\\'が必ずあるようにしておく
	if(!path.empty() && '\\' != path[path.size() - 1]){
		path += '\\';
	}

	// スタックにパスを登録
	vecPath_.push_back(path);

	return true;
}

//------------------------------------------------------------------------------
// スタックから最新の登録パスを取り除く
//------------------------------------------------------------------------------
bool PathManager::PopPath()
{
	if(vecPath_.empty()){ return false; }

	vecPath_.pop_back();

	return true;
}

//------------------------------------------------------------------------------
// 現在のカレントパスを取得する
//------------------------------------------------------------------------------
const char* PathManager::GetPath()const
{
	const static char tmp[] = "";

	if(vecPath_.empty()){ return tmp; }

	return vecPath_.back().c_str();
}
