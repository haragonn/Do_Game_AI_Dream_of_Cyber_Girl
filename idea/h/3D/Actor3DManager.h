/*==============================================================================
	[Actor3DManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_ACTOR3DMANAGER_H
#define INCLUDE_ACTOR3DMANAGER_H

#include "Actor3D.h"

namespace idea{
	//------------------------------------------------------------------------------
	// クラス名　：Actor3DManager
	// クラス概要：シングルトン
	// 　　　　　　Actor3Dの管理を行う
	//------------------------------------------------------------------------------
	class Actor3DManager{
	public:
		// 唯一のインスタンス
		static Actor3DManager& Instance()
		{
			static Actor3DManager s_Instance;
			return s_Instance;
		}

		// 登録
		void Register(Actor3D* pActor3D);

		// 登録解除
		void UnRegister(Actor3D* pActor3D);

		// 座標の保持
		void UpdatePrePosition();

	private:
		// Actor3Dのリスト
		std::vector<Actor3D*> list_;

		// 生成の禁止
		Actor3DManager(){}
		~Actor3DManager(){}

		// コピーコンストラクタの禁止
		Actor3DManager(const Actor3DManager& src){}
		Actor3DManager& operator=(const Actor3DManager& src){}
	};
}

#endif	// #ifndef INCLUDE_ACTOR3DMANAGER_H