/*==============================================================================
	[Actor3DManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_ACTOR3DMANAGER_H
#define INCLUDE_ACTOR3DMANAGER_H

#include "Actor3D.h"

namespace idea{
	//------------------------------------------------------------------------------
	// �N���X���@�FActor3DManager
	// �N���X�T�v�F�V���O���g��
	// �@�@�@�@�@�@Actor3D�̊Ǘ����s��
	//------------------------------------------------------------------------------
	class Actor3DManager{
	public:
		// �B��̃C���X�^���X
		static Actor3DManager& Instance()
		{
			static Actor3DManager s_Instance;
			return s_Instance;
		}

		// �o�^
		void Register(Actor3D* pActor3D);

		// �o�^����
		void UnRegister(Actor3D* pActor3D);

		// ���W�̕ێ�
		void UpdatePrePosition();

	private:
		// Actor3D�̃��X�g
		std::vector<Actor3D*> list_;

		// �����̋֎~
		Actor3DManager(){}
		~Actor3DManager(){}

		// �R�s�[�R���X�g���N�^�̋֎~
		Actor3DManager(const Actor3DManager& src){}
		Actor3DManager& operator=(const Actor3DManager& src){}
	};
}

#endif	// #ifndef INCLUDE_ACTOR3DMANAGER_H