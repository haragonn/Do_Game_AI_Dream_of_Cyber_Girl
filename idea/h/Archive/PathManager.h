#ifndef INCLUDE_IDEA_PATHMANAGER_H
#define INCLUDE_IDEA_PATHMANAGER_H

#include "IPathManager.h"
#include <vector>
#include <string>

namespace idea{
	//------------------------------------------------------------------------------
	// クラス名　：PathManager
	// クラス概要：ファイルパスを管理するクラス
	//------------------------------------------------------------------------------
	class PathManager : public IPathManager
	{
	public:
		// スタックにパスを登録する
		virtual bool PushPath(const char* path);

		
		// スタックから最新の登録パスを取り除く
		virtual bool PopPath();

		
		// 現在のカレントパスを取得する
		virtual const char* GetPath()const;

	private:
		// パスのリスト
		std::vector<std::string> vecPath_;
	};
}

#endif	// #ifndef INCLUDE_IDEA_PATHMANAGER_H