/*==============================================================================
	[Fade.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_FADE_H
#define INCLUDE_IDEA_FADE_H

#include "../Utility/ideaColor.h"

namespace idea{
	//------------------------------------------------------------------------------
	// �N���X���@�FFade
	// �N���X�T�v�F�t�F�[�h�̊Ǘ����s���N���X
	//------------------------------------------------------------------------------
	class Fade{
	public:
		// �t�F�[�h�̐ݒ�
		void SetFade(int time, const Color& color);
		
		// �t�F�[�h�̐ݒ�
		void SetFade(int time, float r = 1.0f, float g = 1.0f, float b = 1.0f);
		
		// �t�F�[�h�C���A�E�g����
		bool IsFading();
		
		// �t�F�[�h�A�E�g������������
		bool IsFadeOutCompleted();
	};
}
#endif	// #ifndef INCLUDE_IDEA_FADE_H