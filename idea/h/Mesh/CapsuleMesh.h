#ifndef INCLUDE_IDEA_CAPSULEMESH_H
#define INCLUDE_IDEA_CAPSULEMESH_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "../3D/Object.h"
#include "../3D/3DStruct.h"
#include "../3D/Collision3D.h"
#include "../Texture/TextureHolder.h"
#include <vector>

//------------------------------------------------------------------------------
// �O���錾
//------------------------------------------------------------------------------
struct ID3D11Buffer;

namespace idea{
	class Camera;
	class ShadowCamera;

	class CapsuleMesh : public Object, public TextureHolder{
	public:
		CapsuleMesh();
		~CapsuleMesh(){ Release(); }

		bool Create(float diameter, float height);

		void Release();

		CapsuleCollider GetCollider();

		// �J�v�Z���R���C�_�[�̎擾
		BaseCollider* GetColliderPtr()override;

		void SetTexture(Texture& tex);

		void ExclusionTexture();

		void SetShadow(ShadowCamera& scmr);

	private:
		float radius_ = 0.0f;
		float height_ = 0.0f;

		ID3D11Buffer* pVertexBuffer_ = nullptr;
		ID3D11Buffer* pIndexBuffer_ = nullptr;

		int indexSize_ = 0;

		CapsuleCollider cc_;

		ShadowCamera* pScmr_;

		void Draw(Camera* pCamera)override;
		inline void DrawCapsule(Camera* pCamera, int blend = 0);	// ��`�̕`��
		inline void DrawShadowCapsule(Camera* pCamera, int blend = 0);	// ��`�̕`��
		inline void DrawTextureCapsule(Camera* pCamera, const Texture& tex, int blend = 0);	// ��`�̕`��
		inline void DrawTextureShadowCapsule(Camera* pCamera, const Texture& tex, int blend = 0);	// ��`�̕`��
	};
}

#endif	// #ifndef INCLUDE_IDEA_CAPSULEMESH_H