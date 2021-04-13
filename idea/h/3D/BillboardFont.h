#ifndef INCLUDE_IDEA_BILLBOARDFONT_H
#define INCLUDE_IDEA_BILLBOARDFONT_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "Object.h"
#include "Billboard.h"
#include "../Texture/Texture.h"
#include "../Utility/ideaColor.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <unordered_map>

struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;

namespace idea{
	class Camera;

	class BillboardFont{
	public:
		BillboardFont();
		~BillboardFont();

		// 初期化
		void Init(const idea::Vector3D& position, Camera& camera, int size, int weight = FW_REGULAR, const char* pFontName = "ＭＳ ゴシック");

		// 終了処理
		void UnInit();

		// 初期化
		void SetPosition(const Vector3D& position);

		// 初期化
		void SetPosition(float positionX, float positionY, float positionZ);

		// 色の設定
		virtual void SetColor(const Color& color);

		// 色の設定
		virtual void SetColor(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);

		//Yビルボードフラグの設定
		void SetYBillboardFlag(bool flag);

		// 書式付きの文字列の描画
		void DrawFormatText(const char* pFormat, ...);

	private:
		// フォントテクスチャ情報
		struct BillboardFontTextureData{
			ID3D11Texture2D* pFontTexture = nullptr;
			ID3D11ShaderResourceView* pTextureView = nullptr;
			int fontWidth = 0;
			int fontHeight = 0;
		};

		// フォントハンドル
		HFONT hf_ = NULL;

		int size_ = 0;

		// 座標
		idea::Vector3D position_;

		// 色
		Color color_;

		// Yビルボードフラグ
		bool bYBillboard_ = false;

		Billboard billboard_;

		bool bAddFlag_ = false;

		// フォントテクスチャのリスト
		std::unordered_map<UINT, BillboardFontTextureData> umTextureData_;

		Texture texture_;

		// 空のテクスチャ情報
		static BillboardFontTextureData s_NullTextureData_;

		// 空のテクスチャ情報
		static Texture pNullTexture_;

		// フォントテクスチャの生成
		BillboardFontTextureData* CreateTextureData(UINT code);

		// キーコードに対するテクスチャを保持しているか
		inline bool HasKeyTextureData(UINT code);

		// コピーコンストラクタの禁止
		BillboardFont(const BillboardFont& src){}
		BillboardFont& operator=(const BillboardFont& src){}
	};
}

#endif	// #ifndef INCLUDE_IDEA_BILLBOARDFONT_H
