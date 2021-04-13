/*==============================================================================
	[SpriteManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_SPRITEMANAGER_H
#define INCLUDE_IDEA_SPRITEMANAGER_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../../../idea/h/Shader/Shader.h"

//------------------------------------------------------------------------------
// 前方宣言
//------------------------------------------------------------------------------
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11ShaderResourceView;
struct ID3D11InputLayout;

namespace idea{
	//------------------------------------------------------------------------------
	// クラス名　：SpriteManager
	// クラス概要：シングルトン,Spriteの描画に必要なリソースを管理するクラス
	//------------------------------------------------------------------------------
	class SpriteManager{
	public:
		// 矩形の頂点数
		static const int RECT_VERTEX_NUM = 4;

		// 円の頂点数
		static const int CIRCLE_VERTEX_NUM = 512;

		// 唯一のインスタンスを返す
		static SpriteManager& Instance()
		{
			static SpriteManager s_Instance;
			return s_Instance;
		}

		// 初期化
		bool Init();

		// 終了処理
		void UnInit();

		// 矩形の頂点バッファのポインタの取得
		inline ID3D11Buffer* GetRectVertexBufferPtr()const
		{
			return pRectVertexBuffer_;
		}

		// 円の頂点バッファのポインタの取得
		inline ID3D11Buffer* GetCircleVertexBufferPtr()const
		{
			return pCircleVertexBuffer_;
		}

		// 円のインデックスバッファのポインタの取得
		inline ID3D11Buffer* GetCircleIndexBufferPtr()const
		{
			return pCircleIndexBuffer_;
		}

		// インプットレイアウトのポインタの取得
		inline ID3D11InputLayout* GetInputLayoutPtr()const
		{
			return pVertexLayout_;
		}

		// 定数バッファのポインタの取得
		inline ID3D11Buffer* GetConstBufferPtr()const
		{
			return pConstBuffer_;
		}

		// 頂点シェーダのポインタの取得
		inline ID3D11VertexShader* GetVertexShederPtr()const
		{
			return pVertexShader_;
		}

		// 通常描画用ピクセルシェーダのポインタの取得
		inline ID3D11PixelShader* GetPixelShederDefaultPtr()const
		{
			return pixelShaderDefault_.GetPixelShaderPtr();
		}

		// テクスチャ描画用ピクセルシェーダのポインタの取得
		inline ID3D11PixelShader* GetPixelShederTexturePtr()const
		{
			return pixelShaderTexture_.GetPixelShaderPtr();
		}

	private:
		// 矩形の頂点バッファのポインタ
		ID3D11Buffer* pRectVertexBuffer_ = nullptr;

		// 円の頂点バッファのポインタ
		ID3D11Buffer* pCircleVertexBuffer_ = nullptr;

		// 円のインデックスバッファのポインタ
		ID3D11Buffer* pCircleIndexBuffer_ = nullptr;

		// インプットレイアウトのポインタ
		ID3D11InputLayout* pVertexLayout_ = nullptr;

		// 定数バッファのポインタ
		ID3D11Buffer* pConstBuffer_ = nullptr;

		// 頂点シェーダのポインタ
		ID3D11VertexShader* pVertexShader_ = nullptr;

		// 通常描画用ピクセルシェーダのポインタ
		PixelShader pixelShaderDefault_;

		// テクスチャ描画用ピクセルシェーダのポインタ
		PixelShader pixelShaderTexture_;

		// 生成の禁止
		SpriteManager();
		~SpriteManager();

		// コピーコンストラクタの禁止
		SpriteManager(const SpriteManager& src){}
		SpriteManager& operator=(const SpriteManager& src){}
	};
}
#endif	// #ifndef INCLUDE_IDEA_SPRITEMANAGER_H