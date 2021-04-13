#ifndef INCLUDE_IDEA_IPATHMANAGER_H
#define INCLUDE_IDEA_IPATHMANAGER_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <memory>

namespace idea{
	//------------------------------------------------------------------------------
	// クラス名　：IPathManager
	// クラス概要：ファイルパスマネージャの基底クラス
	//------------------------------------------------------------------------------
	class IPathManager
	{
	public:
		virtual ~IPathManager(){}

		// スタックにパスを登録する
		virtual bool PushPath(const char* path) = 0;


		// スタックから最新の登録パスを取り除く
		virtual bool PopPath() = 0;


		// 現在のカレントパスを取得する
		virtual const char* GetPath()const = 0;


		// パスを正規化する
		static bool NormalizePath(std::string& path);
	};

	// IPathManagerのshared_ptr
	typedef std::shared_ptr<IPathManager> SPathManager;
}

#endif	// #ifndef INCLUDE_IDEA_IPATHMANAGER_H