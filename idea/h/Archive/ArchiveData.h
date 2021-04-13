#ifndef INCLUDE_IDEA_ARCHIVEDATA_H
#define INCLUDE_IDEA_ARCHIVEDATA_H

#pragma warning (disable:4786)

#include "IFileLoader.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <unordered_map>

namespace idea{
	//------------------------------------------------------------------------------
	// クラス名　：ArchiveData
	// クラス概要：アーカイブファイル上のデータを管理するクラス
	//------------------------------------------------------------------------------
	class ArchiveData : public IFileLoader{
	public:
		// 実体の生成
		static ArchiveData* CreateInstance(const char* archiveFileName);

		// データの読み込み
		virtual bool Load(std::vector<BYTE>& out, const char* filename, DWORD offset = 0, DWORD size = 0)const;

	protected:
		// アーカイブファイルのヘッダー
		typedef struct{
			// ファイル番号
			DWORD dwIndex;

			// ファイル先頭オフセット値
			DWORD dwFileOffset;

			// ファイルサイズ 
			DWORD dwFileSize;
		}FileHeader;

		// ファイルのヘッダーリスト
		typedef std::unordered_map<std::string, FileHeader> FileHeaderList;

		// マウントしたアーカイブファイルネーム
		std::string	archiveFileName_;

		// ファイルのヘッダーリスト
		FileHeaderList mapHeader_;

		ArchiveData(){}
	};
}

#endif	// #ifndef INCLUDE_IDEA_ARCHIVEDATA_H