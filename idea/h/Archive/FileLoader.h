#ifndef INCLUDE_IDEA_FILELOADER_H
#define INCLUDE_IDEA_FILELOADER_H

#include "IFileLoader.h"

namespace idea{
	//------------------------------------------------------------------------------
	// クラス名　：FileLoader
	// クラス概要：ファイル情報を読み込むクラス
	//------------------------------------------------------------------------------
	class FileLoader : public IFileLoader
	{
	public:
		// ファイル情報の読み込み
		virtual bool Load(std::vector<BYTE>& out, const char* filename, DWORD offset = 0, DWORD size = 0)const;
	};
}

#endif	// #ifndef INCLUDE_IDEA_FILELOADER_H