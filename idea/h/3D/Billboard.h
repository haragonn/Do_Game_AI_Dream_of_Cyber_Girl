/*==============================================================================
[Billboard.h]
Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_BILLBOARD_H
#define INCLUDE_IDEA_BILLBOARD_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "Object.h"
#include "../Texture/TextureHolder.h"

namespace idea{
	//------------------------------------------------------------------------------
	// �O���錾
	//------------------------------------------------------------------------------
	class Texture;
	class Camera;

	//------------------------------------------------------------------------------
	// �N���X���@�FCube
	// �N���X�T�v�FActor3D���p�������N���X,�����̂̕`����s��
	//------------------------------------------------------------------------------
	class Billboard : public Object, public TextureHolder{
	public:
		Billboard();
		~Billboard(){}

		// �e�N�X�`���[�̃Z�b�g
		void SetTexture(Texture& tex);

		// ���������ꂽ�e�N�X�`���[�̃Z�b�g
		void SetDividedTexture(Texture& tex, int uNum, int vNum);

		// �e�N�X�`���[�̏��O
		void ExclusionTexture();

		// �u�����h�̐ݒ�
		void SetBlend(int blend)
		{
			blend_ = blend;
		}

		// Y�r���{�[�h�t���O�̐ݒ�
		void SetYBillboardFlag(bool flag)
		{
			bYBillboard_ = flag;
		}

	private:
		// U�ԍ�
		int uNum_ = 0;

		// V�ԍ�
		int vNum_ = 0;

		// �����t���O
		bool bDivided_ = false;

		int blend_ = 0;

		// Y�r���{�[�h�t���O
		bool bYBillboard_ = false;

		// ���[���h�}�g���N�X
		Matrix4x4 billBoardworld_;

		// �`��
		void Draw(Camera* pCamera)override;

		// �`��
		inline void DrawBillboard(Camera* pCamera, int blend = 0);

		// �`��
		inline void DrawTextureBillboard(Camera* pCamera, const Texture& tex, int blend = 0);

		// �`��
		inline void DrawDividedTextureBillboard(Camera* pCamera, const Texture& tex, int uNum, int vNum, int blend = 0);

		// ���[���h�}�g���N�X�̍X�V
		inline void UpdateMatrix(Camera* pCamera);
	};
}
#endif	// #ifndef INCLUDE_IDEA_BILLBOARD_H