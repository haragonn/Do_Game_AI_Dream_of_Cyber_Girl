#ifndef INCLUDE_IDEA_VIRTUALFILELOADER_H
#define INCLUDE_IDEA_VIRTUALFILELOADER_H

#include "IFileLoader.h"
#include "PathManager.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace idea{
	//------------------------------------------------------------------------------
	// クラス名　：VirtualFileLoader
	// クラス概要：複数フォルダに散っているファイルを,1つのフォルダ内に存在するかの
	// 　　　　　　ように仮想的に扱えるファイルローダ,パス管理とロード機能も持つ
	//------------------------------------------------------------------------------
	class VirtualFileLoader : public IFileLoader, public PathManager{
	public:
		// 新しいファイルローダを登録する
		virtual bool PushLoader(SFileLoader loader, const char* path);

		// 最後に登録されたファイルローダをスタックから下ろす
		virtual bool PopLoader();

		// ファイルを読み込む
		virtual bool Load(std::vector<BYTE>& out, const char* filename, DWORD offset = 0, DWORD size = 0)const;

		// ローダー情報
		struct LoaderPair
		{
			LoaderPair(SFileLoader loader, std::string path);

			// ローダー
			SFileLoader loader;

			// 基点パス名
			std::string	path;
		};

		// ローダーのリスト
		std::vector<LoaderPair> vecLoader_;
	};
}

#endif	// INCLUDE_IDEA_VIRTUALFILELOADER_H
