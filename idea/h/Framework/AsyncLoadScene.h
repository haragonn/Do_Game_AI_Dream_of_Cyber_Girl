/*==============================================================================
	[AsyncLoadScene.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_ASYNCLOADSCENE_H
#define INCLUDE_IDEA_ASYNCLOADSCENE_H

#include "Scene.h"
#include <atomic>

namespace idea{
	//------------------------------------------------------------------------------
	// �N���X���@�FAsyncScene
	// �N���X�T�v�F�񓯊��ǂݍ��ݕt���̃Q�[���V�[���N���X�̊��ƂȂ�N���X
	//------------------------------------------------------------------------------
	class AsyncLoadScene : public Scene{
	public:
		AsyncLoadScene() : bLoadCompleted_(false){ bAsync_ = true; }
		virtual ~AsyncLoadScene(){}

		// �񓯊��ǂݍ���
		virtual void Load() = 0;

		// �񓯊��ǂݍ��݂�����������
		bool IsLoadCompleted()const{ return bLoadCompleted_; }

	private:
		friend class SequenceManager;

		// �񓯊��ǂݍ��݊����t���O
		std::atomic<bool> bLoadCompleted_;
	};
}

#endif	// #ifndef INCLUDE_IDEA_ASYNCLOADSCENE_H