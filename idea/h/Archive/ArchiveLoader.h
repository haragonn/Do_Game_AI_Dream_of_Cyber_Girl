/*==============================================================================
	[ArchiveLoader.cpp]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_ARCHIVELOADER_H
#define INCLUDE_IDEA_ARCHIVELOADER_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vector>

namespace idea{
	//------------------------------------------------------------------------------
	// クラス名　：ArchiveLoader
	// クラス概要：アーカイブファイルの読み込み,管理を行うクラス
	//------------------------------------------------------------------------------
	class ArchiveLoader{
	public:
		ArchiveLoader(){}
		~ArchiveLoader(){ UnLoad(); }

		// アーカイブファイルの読み込み
		bool Load(const char* archiveFileName, const char* fileName);

		// データの取得
		std::vector<BYTE>& GetData(){ return data_; }

		// データのサイズの取得
		size_t GetSize(){ return data_.size(); }

		// データの破棄
		void UnLoad();

	private:
		// データ
		std::vector<BYTE> data_;

		// コピーコンストラクタの禁止
		ArchiveLoader(const ArchiveLoader& src){}
		ArchiveLoader& operator=(const ArchiveLoader& src){}
	};
}

#endif	// #ifndef INCLUDE_IDEA_ARCHIVELOADER_H