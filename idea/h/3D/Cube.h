/*==============================================================================
	[Cube.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_CUBE_H
#define INCLUDE_IDEA_CUBE_H

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
	// クラス名　：Cube
	// クラス概要：Actor3Dを継承したクラス,立方体の描画を行う
	//------------------------------------------------------------------------------
	class Cube : public Object, public TextureHolder{
	public:
		Cube();
		virtual ~Cube(){}

		// ボックスコライダーの取得
		BoxCollider GetCollider();

		// ボックスコライダーの取得
		BaseCollider* GetColliderPtr()override;

		// テクスチャーのセット
		void SetTexture(Texture& tex);

		// 等分割されたテクスチャーのセット
		void SetDividedTexture(Texture& tex, int uNum, int vNum);

		// 表示範囲を指定したテクスチャーの描画
		void SetDelimitedTexture(Texture& tex, float u, float v, float width, float height);

		// テクスチャーの除外
		void ExclusionTexture();

		// シャドーカメラのセット
		void SetShadow(ShadowCamera& scmr);

	private:
		// U番号
		int uNum_ = 0;

		// V番号
		int vNum_ = 0;

		// UV値
		Vector2D uv_;

		// 表示範囲
		Vector2D size_;

		// 分割フラグ
		bool bDivided_ = false;

		// 範囲設定フラグ
		bool bDelimited_ = false;

		BoxCollider bc_;

		// シャドウカメラのポインタ―
		ShadowCamera* pScmr_ = nullptr;

		// 描画
		void Draw(Camera* pCamera)override;

		// 描画
		inline void DrawCube(Camera* pCamera, int blend = 0);

		// 描画
		inline void DrawShadowCube(Camera* pCamera, int blend = 0);

		// 描画
		inline void DrawTextureCube(Camera* pCamera, const Texture& tex, int blend = 0);

		// 描画
		inline void DrawTextureShadowCube(Camera* pCamera, const Texture& tex, int blend = 0);

		// 描画
		inline void DrawDividedTextureCube(Camera* pCamera, const Texture& tex, int uNum, int vNum, int blend = 0);

		// 描画
		inline void DrawDividedTextureShadowCube(Camera* pCamera, const Texture& tex, int uNum, int vNum, int blend = 0);

		// 描画
		inline void DrawDelimitedTextureCube(Camera* pCamera, const Texture& tex, float u, float v, float width, float height, int blend = 0);

		// 描画
		inline void DrawDelimitedTextureShadowCube(Camera* pCamera, const Texture& tex, float u, float v, float width, float height, int blend = 0);
	};
}
#endif	// #ifndef INCLUDE_IDEA_CUBE_H
