/*==============================================================================
[Billboard.h]
Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_BILLBOARD_H
#define INCLUDE_IDEA_BILLBOARD_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "Object.h"
#include "../Texture/TextureHolder.h"

namespace idea{
	//------------------------------------------------------------------------------
	// 前方宣言
	//------------------------------------------------------------------------------
	class Texture;
	class Camera;

	//------------------------------------------------------------------------------
	// クラス名　：Cube
	// クラス概要：Actor3Dを継承したクラス,立方体の描画を行う
	//------------------------------------------------------------------------------
	class Billboard : public Object, public TextureHolder{
	public:
		Billboard();
		~Billboard(){}

		// テクスチャーのセット
		void SetTexture(Texture& tex);

		// 等分割されたテクスチャーのセット
		void SetDividedTexture(Texture& tex, int uNum, int vNum);

		// テクスチャーの除外
		void ExclusionTexture();

		// ブレンドの設定
		void SetBlend(int blend)
		{
			blend_ = blend;
		}

		// Yビルボードフラグの設定
		void SetYBillboardFlag(bool flag)
		{
			bYBillboard_ = flag;
		}

	private:
		// U番号
		int uNum_ = 0;

		// V番号
		int vNum_ = 0;

		// 分割フラグ
		bool bDivided_ = false;

		int blend_ = 0;

		// Yビルボードフラグ
		bool bYBillboard_ = false;

		// ワールドマトリクス
		Matrix4x4 billBoardworld_;

		// 描画
		void Draw(Camera* pCamera)override;

		// 描画
		inline void DrawBillboard(Camera* pCamera, int blend = 0);

		// 描画
		inline void DrawTextureBillboard(Camera* pCamera, const Texture& tex, int blend = 0);

		// 描画
		inline void DrawDividedTextureBillboard(Camera* pCamera, const Texture& tex, int uNum, int vNum, int blend = 0);

		// ワールドマトリクスの更新
		inline void UpdateMatrix(Camera* pCamera);
	};
}
#endif	// #ifndef INCLUDE_IDEA_BILLBOARD_H