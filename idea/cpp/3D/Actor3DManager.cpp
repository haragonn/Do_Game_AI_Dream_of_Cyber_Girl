/*==============================================================================
	[Actor3DManager.cpp]
														Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../../h/3D/Actor3DManager.h"
#include "../../h/3D/Actor3D.h"

using namespace idea;

//------------------------------------------------------------------------------
// 登録
//------------------------------------------------------------------------------
void Actor3DManager::Register(Actor3D* pActor3D)
{
	// NULLチェック,登録済みチェック
	if(!pActor3D || pActor3D->bRegistered_){
		return;
	}

	list_.push_back(pActor3D);

	pActor3D->bRegistered_ = true;	// 登録済みに
}

//------------------------------------------------------------------------------
// 登録解除
//------------------------------------------------------------------------------
void Actor3DManager::UnRegister(Actor3D* pActor3D)
{
	// NULLチェック,削除済みチェック
	if(!pActor3D || !pActor3D->bRegistered_){
		return;
	}
 
	auto actor = std::find(list_.begin(), list_.end(), pActor3D);

	if(actor != list_.end()){
		list_.erase(actor);
	}

	pActor3D->bRegistered_ = false;	// 削除済みに
}

//------------------------------------------------------------------------------
// 座標の保持
//------------------------------------------------------------------------------
void Actor3DManager::UpdatePrePosition()
{
	for(auto actor : list_){
		actor->UpdatePrePosition();
	}
}
