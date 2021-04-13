/*==============================================================================
	[Cube.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_CUBE_H
#define INCLUDE_IDEA_CUBE_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "Object.h"
#include "Collision3D.h"
#include "../Texture/TextureHolder.h"
#include "../Utility/ideaMath.h"

namespace idea{
	//------------------------------------------------------------------------------
	// �O���錾
	//------------------------------------------------------------------------------
	class Texture;
	class Camera;
	class ShadowCamera;

	//------------------------------------------------------------------------------
	// �N���X���@�FCube
	// �N���X�T�v�FActor3D���p�������N���X,�����̂̕`����s��
	//------------------------------------------------------------------------------
	class Cube : public Object, public TextureHolder{
	public:
		Cube();
		virtual ~Cube(){}

		// �{�b�N�X�R���C�_�[�̎擾
		BoxCollider GetCollider();

		// �{�b�N�X�R���C�_�[�̎擾
		BaseCollider* GetColliderPtr()override;

		// �e�N�X�`���[�̃Z�b�g
		void SetTexture(Texture& tex);

		// ���������ꂽ�e�N�X�`���[�̃Z�b�g
		void SetDividedTexture(Texture& tex, int uNum, int vNum);

		// �\���͈͂��w�肵���e�N�X�`���[�̕`��
		void SetDelimitedTexture(Texture& tex, float u, float v, float width, float height);

		// �e�N�X�`���[�̏��O
		void ExclusionTexture();

		// �V���h�[�J�����̃Z�b�g
		void SetShadow(ShadowCamera& scmr);

	private:
		// U�ԍ�
		int uNum_ = 0;

		// V�ԍ�
		int vNum_ = 0;

		// UV�l
		Vector2D uv_;

		// �\���͈�
		Vector2D size_;

		// �����t���O
		bool bDivided_ = false;

		// �͈͐ݒ�t���O
		bool bDelimited_ = false;

		BoxCollider bc_;

		// �V���h�E�J�����̃|�C���^�\
		ShadowCamera* pScmr_ = nullptr;

		// �`��
		void Draw(Camera* pCamera)override;

		// �`��
		inline void DrawCube(Camera* pCamera, int blend = 0);

		// �`��
		inline void DrawShadowCube(Camera* pCamera, int blend = 0);

		// �`��
		inline void DrawTextureCube(Camera* pCamera, const Texture& tex, int blend = 0);

		// �`��
		inline void DrawTextureShadowCube(Camera* pCamera, const Texture& tex, int blend = 0);

		// �`��
		inline void DrawDividedTextureCube(Camera* pCamera, const Texture& tex, int uNum, int vNum, int blend = 0);

		// �`��
		inline void DrawDividedTextureShadowCube(Camera* pCamera, const Texture& tex, int uNum, int vNum, int blend = 0);

		// �`��
		inline void DrawDelimitedTextureCube(Camera* pCamera, const Texture& tex, float u, float v, float width, float height, int blend = 0);

		// �`��
		inline void DrawDelimitedTextureShadowCube(Camera* pCamera, const Texture& tex, float u, float v, float width, float height, int blend = 0);
	};
}
#endif	// #ifndef INCLUDE_IDEA_CUBE_H
