#ifndef INCLUDE_IDEA_SPHERE_H
#define INCLUDE_IDEA_SPHERE_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "Object.h"
#include "Collision3D.h"
#include "../Texture/TextureHolder.h"
#include "../Utility/ideaMath.h"

namespace idea{
	//------------------------------------------------------------------------------
	// 前方宣言
	//------------------------------------------------------------------------------
	class Texture;
	class Camera;
	class ShadowCamera;

	//------------------------------------------------------------------------------
	// クラス名　：Sphere
	// クラス概要：Actor3Dを継承したクラス,球体の描画を行う
	//------------------------------------------------------------------------------
	class Sphere : public Object, public idea::TextureHolder{
	public:
		Sphere();

		// スフィアコライダーの取得
		SphereCollider GetCollider();

		// ボックスコライダーの取得
		BaseCollider* GetColliderPtr()override;

		// テクスチャーのセット
		void SetTexture(idea::Texture& tex);

		// テクスチャーの除外
		void ExclusionTexture();

		// シャドーカメラのセット
		void SetShadow(ShadowCamera& scmr);

	private:
		// スフィアコライダー
		SphereCollider sc_;

		// シャドウカメラのポインタ―
		ShadowCamera* pScmr_ = nullptr;

		// 描画
		void Draw(Camera* pCamera)override;

		// 描画
		inline void DrawSphere(Camera* pCamera, int blend = 0);

		// 描画
		inline void DrawShadowSphere(Camera* pCamera, int blend = 0);

		// 描画
		inline void DrawTextureSphere(Camera* pCamera, const idea::Texture& tex, int blend = 0);

		// 描画
		inline void DrawTextureShadowSphere(Camera* pCamera, const idea::Texture& tex, int blend = 0);
	};
}

#endif	// #ifndef INCLUDE_SPHERE_H