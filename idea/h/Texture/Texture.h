/*==============================================================================
	[Texture.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_TEXTURE_H
#define INCLUDE_IDEA_TEXTURE_H

#include <vector>

struct ID3D11Resource;
struct ID3D11ShaderResourceView;

namespace idea{
	class TextureHolder;

	//------------------------------------------------------------------------------
	// クラス名　：Texture
	// クラス概要：画像の読み込み,テクスチャリソースの管理を行う
	//------------------------------------------------------------------------------
	class Texture{
	public:
		Texture();
		~Texture();

		// 画像の読み込み
		bool LoadImageFromFile(const char* pFileName, unsigned int divX = 1U, unsigned int divY = 1U);
		
		// アーカイブファイルから画像を読み込む
		bool LoadImageFromArchiveFile(const char* pArchiveFileName, const char* pFileName, unsigned int divX = 1U, unsigned int divY = 1U);

		// ストレージから画像を読み込む
		bool LoadImageFromStorage(const char* pFileName, unsigned int divX = 1U, unsigned int divY = 1U);

		// ストレージから画像を読み込む
		bool LoadImageFromTextureView(ID3D11ShaderResourceView* textureView, unsigned int divX = 1U, unsigned int divY = 1U);

		// 画像の破棄
		void UnLoad();

		// テクスチャリソースビューのポインタ―の取得
		ID3D11ShaderResourceView* GetTextureViewPtr()const{ return pTextureView_; }

		// 分割数の取得(横)
		float GetDivU()const{ return divU_; }

		// 分割数の取得(縦)
		float GetDivV()const{ return divV_; }

	private:
		friend class StorageManager;
		friend class TextureHolder;

		// ファイル名
		char fileName_[256] = {};

		// テクスチャリソースビューのポインタ
		ID3D11ShaderResourceView* pTextureView_ = nullptr;

		// 分割数(横)
		float divU_ = 0U;
		
		// 分割数(縦)
		float divV_ = 0U;

		// ホルダーリスト
		std::vector<TextureHolder*> vecTexHolderPtr_;

		// ストレージ使用フラグ
		bool bStorage_ = false;

		// コピーコンストラクタの禁止
		Texture(const Texture& src){}
		Texture& operator=(const Texture& src){}
	};
}
#endif	// #ifndef INCLUDE_IDEA_TEXTURE_H
