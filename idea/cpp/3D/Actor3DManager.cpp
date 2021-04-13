/*==============================================================================
	[Actor3DManager.cpp]
														Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "../../h/3D/Actor3DManager.h"
#include "../../h/3D/Actor3D.h"

using namespace idea;

//------------------------------------------------------------------------------
// �o�^
//------------------------------------------------------------------------------
void Actor3DManager::Register(Actor3D* pActor3D)
{
	// NULL�`�F�b�N,�o�^�ς݃`�F�b�N
	if(!pActor3D || pActor3D->bRegistered_){
		return;
	}

	list_.push_back(pActor3D);

	pActor3D->bRegistered_ = true;	// �o�^�ς݂�
}

//------------------------------------------------------------------------------
// �o�^����
//------------------------------------------------------------------------------
void Actor3DManager::UnRegister(Actor3D* pActor3D)
{
	// NULL�`�F�b�N,�폜�ς݃`�F�b�N
	if(!pActor3D || !pActor3D->bRegistered_){
		return;
	}
 
	auto actor = std::find(list_.begin(), list_.end(), pActor3D);

	if(actor != list_.end()){
		list_.erase(actor);
	}

	pActor3D->bRegistered_ = false;	// �폜�ς݂�
}

//------------------------------------------------------------------------------
// ���W�̕ێ�
//------------------------------------------------------------------------------
void Actor3DManager::UpdatePrePosition()
{
	for(auto actor : list_){
		actor->UpdatePrePosition();
	}
}
