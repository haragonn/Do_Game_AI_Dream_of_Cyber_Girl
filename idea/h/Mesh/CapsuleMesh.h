#ifndef INCLUDE_IDEA_CAPSULEMESH_H
#define INCLUDE_IDEA_CAPSULEMESH_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../3D/Object.h"
#include "../3D/3DStruct.h"
#include "../3D/Collision3D.h"
#include "../Texture/TextureHolder.h"
#include <vector>

//------------------------------------------------------------------------------
// 前方宣言
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

		// カプセルコライダーの取得
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
		inline void DrawCapsule(Camera* pCamera, int blend = 0);	// 矩形の描画
		inline void DrawShadowCapsule(Camera* pCamera, int blend = 0);	// 矩形の描画
		inline void DrawTextureCapsule(Camera* pCamera, const Texture& tex, int blend = 0);	// 矩形の描画
		inline void DrawTextureShadowCapsule(Camera* pCamera, const Texture& tex, int blend = 0);	// 矩形の描画
	};
}

#endif	// #ifndef INCLUDE_IDEA_CAPSULEMESH_H