/*==============================================================================
	[FadeManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_FADEMANAGER_H
#define INCLUDE_IDEA_FADEMANAGER_H

#include "../2D/Sprite.h"
#include "../Texture/Texture.h"

namespace idea{
	//------------------------------------------------------------------------------
	// �N���X���@�FFadeManager
	// �N���X�T�v�F�V���O���g��
	// �@�@�@�@�@�@�t�F�[�h�̐ݒ�,�`����s��
	//------------------------------------------------------------------------------
	class FadeManager{
	public:
		// �B��̃C���X�^���X��Ԃ�
		static FadeManager& Instance()
		{
			static FadeManager s_Instance;
			return s_Instance;
		}

		// ������
		void Init(float width, float height);
		
		// �I������
		void UnInit();

		// �t�F�[�h�̐ݒ�
		void SetFade(int time, float r = 1.0f, float g = 1.0f, float b = 1.0f);

		// �X�V
		void Update();
		
		// �t�F�[�h�����ǂ���
		bool IsFading()const{ return !!(int)(a_ + 0.999999f); }
		
		// �t�F�[�h�A�E�g�������������ǂ���
		bool IsFadeOutCompleted()const{ return bFadeOutCompleted_; }

		// �`��
		void Draw();

	private:
		// �X�v���C�g
		Sprite spr_;

		// �Ԑ���
		float r_ = 0.0f;

		// �ΐ���
		float g_ = 0.0f;

		// ����
		float b_ = 0.0f;

		// a�l
		float a_ = 0.0f;

		// �ω���
		float value_ = 0.0f;

		// �L�����ǂ���
		bool bFadeEnable_ = false;
		
		// �t�F�[�h�A�E�g�t���O
		bool bFadeOut_ = false;
		
		// �t�F�[�h�A�E�g�����t���O
		bool bFadeOutCompleted_ = false;

		FadeManager() :
			r_(1.0f),
			g_(1.0f),
			b_(1.0f),
			a_(0.0f),
			value_(1.0f),
			bFadeEnable_(false),
			bFadeOut_(false),
			bFadeOutCompleted_(false){}

		~FadeManager(){}

		// �R�s�[�R���X�g���N�^�̋֎~
		FadeManager(const FadeManager& src){}
		FadeManager& operator =(const FadeManager& src){}
	};
}

#endif // #ifndef INCLUDE_IDEA_FADEMANAGER_H