#ifndef INCLUDE_IDEA_IFILELOADER_H
#define INCLUDE_IDEA_IFILELOADER_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vector>
#include <memory>

namespace idea{
	//------------------------------------------------------------------------------
	// クラス名　：IFileLoader
	// クラス概要：ファイルローダーの基底クラス
	//------------------------------------------------------------------------------
	class IFileLoader
	{
	public:
		virtual ~IFileLoader(){};

		// ファイルをロードする
		virtual bool Load(std::vector<BYTE>& out, const char* filename, DWORD offset = 0U, DWORD size = 0U)const = 0;
	};

	// IFileLoaderのshared_ptr
	typedef std::shared_ptr<IFileLoader> SFileLoader;
}

#endif	// #ifndef INCLUDE_IDEA_IFILELOADER_H