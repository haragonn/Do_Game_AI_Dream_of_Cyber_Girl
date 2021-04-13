#ifndef INCLUDE_IDEA_SPHERE_H
#define INCLUDE_IDEA_SPHERE_H

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
	// �N���X���@�FSphere
	// �N���X�T�v�FActor3D���p�������N���X,���̂̕`����s��
	//------------------------------------------------------------------------------
	class Sphere : public Object, public idea::TextureHolder{
	public:
		Sphere();

		// �X�t�B�A�R���C�_�[�̎擾
		SphereCollider GetCollider();

		// �{�b�N�X�R���C�_�[�̎擾
		BaseCollider* GetColliderPtr()override;

		// �e�N�X�`���[�̃Z�b�g
		void SetTexture(idea::Texture& tex);

		// �e�N�X�`���[�̏��O
		void ExclusionTexture();

		// �V���h�[�J�����̃Z�b�g
		void SetShadow(ShadowCamera& scmr);

	private:
		// �X�t�B�A�R���C�_�[
		SphereCollider sc_;

		// �V���h�E�J�����̃|�C���^�\
		ShadowCamera* pScmr_ = nullptr;

		// �`��
		void Draw(Camera* pCamera)override;

		// �`��
		inline void DrawSphere(Camera* pCamera, int blend = 0);

		// �`��
		inline void DrawShadowSphere(Camera* pCamera, int blend = 0);

		// �`��
		inline void DrawTextureSphere(Camera* pCamera, const idea::Texture& tex, int blend = 0);

		// �`��
		inline void DrawTextureShadowSphere(Camera* pCamera, const idea::Texture& tex, int blend = 0);
	};
}

#endif	// #ifndef INCLUDE_SPHERE_H