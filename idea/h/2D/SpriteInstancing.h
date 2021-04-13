/*==============================================================================
	[SpriteInstancing.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_SPRITEINSTANCING_H
#define INCLUDE_IDEA_SPRITEINSTANCING_H

#include "../Utility/ideaMath.h"
#include "../Utility/ideaColor.h"
#include <vector>

namespace idea{
	//------------------------------------------------------------------------------
	// 前方宣言
	//------------------------------------------------------------------------------
	class Sprite;
	class Texture;

	//------------------------------------------------------------------------------
	// クラス名　：SpriteInstancing
	// クラス概要：Spriteを格納し,一度に同じ画像を表示するクラス
	//------------------------------------------------------------------------------
	class SpriteInstancing{
	public:
		SpriteInstancing();
		~SpriteInstancing();

		// 初期化
		void Init(const Vector2D& size);

		// 初期化
		void Init(float width, float height);

		// 終了処理
		void UnInit();

		// サイズの設定
		void SetSize(const Vector2D& size);

		// サイズの設定
		void SetSize(float width, float height);

		// サイズの取得
		inline const Vector2D& GetSize()const
		{
			return size_;
		}

		// 回転角の設定
		void SetRotation(float rad);

		// 回転
		void MoveRotation(float rad);

		// 回転角の取得
		inline float GetRoteate()const
		{
			return radian_;
		}

		// 色の設定
		void SetColor(const Color& color);

		// 色の設定
		void SetColor(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);

		// 色の取得
		inline const Color& GetColor()const
		{
			return color_;
		}

		// 描画するスプライトの追加
		void AddSprite(Sprite& sprite);

		// 描画するスプライトの削除
		void RemoveSprite(Sprite& sprite);

		// 描画する全スプライトを削除
		void ResetSprite();
	
		// 矩形の描画
		void DrawRect(int blend = 0);

		// テクスチャの描画
		void DrawTexture(const Texture& tex, int blend = 0);

		// 等分割されたテクスチャの描画
		void DrawDividedTexture(const Texture& tex, int uNum, int vNum, int blend = 0);

		// 表示範囲を指定したテクスチャーの描画
		void DrawDelimitedTexture(const Texture& tex, float u, float v, float width, float height, int blend = 0);

	private:
		// 大きさ
		Vector2D size_;

		// 角度
		float radian_ = 0.0f;

		// 色
		Color color_;

		// 反転フラグ(U)
		bool bReversedU_ = false;

		// 反転フラグ(V)
		bool bReversedV_ = false;

		// 描画するスプライトのリスト
		std::vector<Sprite*> vecSpritePtr_;
	};
}

#endif	// #ifndef INCLUDE_IDEA_SPRITEINSTANCING_H