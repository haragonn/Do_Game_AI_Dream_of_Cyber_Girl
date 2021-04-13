/*==============================================================================
	[FadeManager.cpp]
														Author	:	Keigo Hara
==============================================================================*/
#include "../../h/Framework/FadeManager.h"

using namespace idea;

//------------------------------------------------------------------------------
// ������
//------------------------------------------------------------------------------
void FadeManager::Init(float width, float height)
{
	spr_.Init(width * 0.5f, height * 0.5f, width, height);	// �X�v���C�g�̏�����
}

//------------------------------------------------------------------------------
// �I������
//------------------------------------------------------------------------------
void FadeManager::UnInit()
{
	// �K���Ȓl�Ō�n��
	r_ = 0;
	g_ = 0;
	b_ = 0;
	a_ = 0.0f;

	value_ = 1.0f;

	bFadeEnable_ = false;
	bFadeOut_ = false;
	bFadeOutCompleted_ = false;
}

//------------------------------------------------------------------------------
// �t�F�[�h�̐ݒ�
//------------------------------------------------------------------------------
void FadeManager::SetFade(int time, float r, float g, float b)
{
	if(bFadeEnable_){ return; }	// �g�p���Ȃ�I��

	// �����̔��f
	r_ = r;
	g_ = g;
	b_ = b;
	a_ = 0.0f;

	value_ = 1.0f / time;

	// �t���O���I����
	bFadeEnable_ = true;
	bFadeOut_ = true;
}

//------------------------------------------------------------------------------
// �X�V
//------------------------------------------------------------------------------
void FadeManager::Update()
{
	if(!bFadeEnable_){ return; }	// �L���łȂ��Ȃ�I��

	bFadeOutCompleted_ = false;	// �t�F�[�h�A�E�g�����t���O���I�t��

	if(bFadeOut_){	// �t�F�[�h�A�E�g
		a_ += value_;

		if(a_ >= 1.0f){
			a_ = 1.0f;
			bFadeOut_ = false;
			bFadeOutCompleted_ = true;
		}
	} else{			// �t�F�[�h�C��
		a_ -= value_;

		if(a_ <= 0.0f){
			a_ = 0.0f;
			bFadeEnable_ = false;
		}
	}

	spr_.SetColor(r_, g_, b_, a_ * a_ * (3.0f - 2.0f * a_));	// ease_in_ease_out
}

//------------------------------------------------------------------------------
// �`��
//------------------------------------------------------------------------------
void FadeManager::Draw()
{
	if(!bFadeEnable_){ return; }	// �L���łȂ��Ȃ�I��

	spr_.DrawRect();	// �`��
}