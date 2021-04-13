#include "../../h/Archive/IPathManager.h"
#include <shlwapi.h>
#include <vector>

#pragma comment(lib, "shlwapi.lib")

using namespace idea;

//------------------------------------------------------------------------------
// パスを正規化する
//------------------------------------------------------------------------------
bool IPathManager::NormalizePath(std::string& path)
{
	// パスをコピー
	std::string tmp = path;

	// 全ての'/'を,'\\'に置き換えることで,パス記法に統一性を持たせる
	int n = 0;
	while(-1 != (n = (int)tmp.find('/', n))){
		tmp = tmp.replace(n, 1, "\\");
	}

	// 不要な"."や".."などの文字を含むパス名を,これらの文字を含まないパス名に変換する
	std::vector<char> p(tmp.size() + 1);

	if(!::PathCanonicalize(&p[0], tmp.c_str())){
		return false;
	}

	path = std::string(&p[0]);

	return true;
}