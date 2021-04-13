/*==============================================================================
	[Font.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_FONT_H
#define INCLUDE_IDEA_FONT_H

#include "../Utility/ideaColor.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <unordered_map>

struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;

namespace idea{
	// フォントの基準点
	enum class FontReferencePoint{
		UPPER_LEFT = 0,
		LEFT_CENTER,
		LOWER_LEFT,
		CENTER,
		UPPER_RIGHT,
		RIGHT_CENTER,
		LOWER_RIGHT,
		FONT_REFERENCE_POINT_MAX
	};

	//------------------------------------------------------------------------------
	// クラス名　：Font
	// クラス概要：APIからフォントのテクスチャを作成し,描画するクラス
	//------------------------------------------------------------------------------
	class Font{
	public:
		Font();
		~Font();

		// 初期化
		void Init(int size, int weight = FW_REGULAR, const FontReferencePoint point = FontReferencePoint::UPPER_LEFT, const char* pFontName = "ＭＳ ゴシック");

		// 終了処理
		void UnInit();

		// 文字の大きさの取得
		inline int GetSize()const{ return size_; }

		// 色の設定
		void SetColor(const Color& color);

		// 色の設定
		void SetColor(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);

		// 色の取得
		inline const Color& GetColor()const{ return color_; }

		// 書式付きの文字列の描画
		void DrawFormatText(float posX, float posY, const char* pFormat, ...);

	private:
		// フォントテクスチャ情報
		typedef struct{
			ID3D11Texture2D* pFontTexture;
			ID3D11ShaderResourceView* pTextureView;
			int fontWidth;
			int fontHeight;
		}FontTextureData;

		// フォントハンドル
		HFONT hf_ = NULL;

		// 色
		Color color_;

		// 文字の大きさ
		int size_ = 0;

		// 基準点
		FontReferencePoint point_ = FontReferencePoint::UPPER_LEFT;

		// フォントテクスチャのリスト
		std::unordered_map<UINT, FontTextureData> umTextureData_;

		// 空のテクスチャ情報
		static FontTextureData s_NullTextureData_;

		// フォントテクスチャの生成
		FontTextureData* CreateTextureData(UINT code);

		// キーコードに対するテクスチャを保持しているか
		inline bool HasKeyTextureData(UINT code);

		// コピーコンストラクタの禁止
		Font(const Font& src){}
		Font& operator=(const Font& src){}
	};
}

#endif	// #ifndef INCLUDE_IDEA_FONT_H
