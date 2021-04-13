/*==============================================================================
	[GraphicManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_GRAPHICMANAGER_H
#define INCLUDE_IDEA_GRAPHICMANAGER_H

#include "../Utility/ideaMath.h"
#include "../Utility/ideaUtility.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>

namespace idea{
	// ブレンドステート
	enum class BLEND_STATE{
		// 定義なし
		BLEND_NONE = 0,

		// アルファブレンドなし
		BLEND_DEFAULT,

		// アルファブレンドあり
		BLEND_ALIGNMENT,

		// 加算合成
		BLEND_ADD,

		// 減算合成
		BLEND_SUBTRACT,

		// 乗算合成
		BLEND_MULTIPLE,

		// 最大値
		BLEND_MAX
	};

	struct PeraVertex{
		// 座標
		Vector3D pos;

		// テクスチャ座標
		Vector2D tex;
	};

	//------------------------------------------------------------------------------
	// クラス名　：GraphicManager
	// クラス概要：シングルトン
	// 　　　　　　デバイス,コンテキスト,デプスバッファ,ラスタライザの管理を行う
	//------------------------------------------------------------------------------
	class GraphicManager{
	public:
		// レンダーターゲットビューの数
		static const int RENDER_TARGET_VIEW_MAX = 4;

		// 唯一のインスタンスを返す
		static GraphicManager& Instance()
		{
			static GraphicManager s_Instance;
			return s_Instance;
		}

		// 初期化
		bool Init(HWND hWnd, UINT width, UINT height, bool bWindowed, UINT fps = 60U, bool bMSAA = false);

		// 終了処理
		void UnInit();

		// 描画開始
		bool BeginScene();

		// 描画終了
		bool EndScene();

		// パスの描画
		bool DrawPath(int target, int src, ID3D11PixelShader* pps, D3D11_VIEWPORT viewPort);

		// デプスバッファの描画
		bool DrawShadow(int target, D3D11_VIEWPORT viewPort);

		// ウィンドウハンドルの取得
		HWND GetHWND()const{ return hWnd_; }

		// スクリーン幅の取得
		int	GetWidth()const{ return width_; }

		// スクリーン高さの取得
		int GetHeight()const{ return height_; }

		// Direct3Dデバイスの取得
		ID3D11Device* GetDevicePtr()const{ return pD3DDevice_; }

		// コンテキストの取得
		ID3D11DeviceContext* GetContextPtr()const{ return pImmediateContext_; }

		// デプスステンシルビューの取得
		ID3D11DepthStencilView* GetDepthStencilViewPtr(){ return pDepthStencilView_; }

		// デプスステンシルビューの取得
		ID3D11ShaderResourceView* GetDepthShaderResourceViewPtr(){ return pDepthShaderResourceView_; }

		// 影描画ペラ用のシェーダポインタの取得
		ID3D11PixelShader* GetPeraShadowPixelShaderPtr(){
			return pPeraShadowPixelShader_;
		}

		// レンダーターゲットビューポインタの取得
		ID3D11RenderTargetView* GetRenderTargetViewPtr(int idx)
		{
			Clamp(idx, 0, RENDER_TARGET_VIEW_MAX - 1);
			return pRenderTargetViews_[idx];
		}

		// レンダーターゲットビュー配列ポインタの取得
		ID3D11ShaderResourceView** GetShaderResourceViewPtrArray()
		{
			return pShaderResourceViews_;
		}

		// ペラポリゴンの頂点バッファポインタの取得
		ID3D11Buffer* GetPeraVertexBufferPtr()const{ return pPeraVertexBuffer_; }

		// 標準ラスタライザ―ステートの取得
		ID3D11RasterizerState* GetDefaultRasterizerStatePtr(){ return pRsState_; }

		// 標準深度ステートの取得
		ID3D11DepthStencilState* GetDefaultDepthStatePtr(){ return pDsState_; }

		// ブレンドステートを設定する
		bool SetBlendState(BLEND_STATE blendState);

		// マスクの描画
		bool DrawMask(bool bVisible);

		// マスクのAND判定
		bool DrawAnd();

		// マスクのXOR判定
		bool DrawXor();

		// マスクの終了
		bool EndMask();

		// 3D描画の準備
		bool Draw3D();

		// ウィンドウモードの切り替え
		bool ChangeDisplayMode(bool bWindowed);

	private:
		// ウィンドウハンドル
		HWND hWnd_ = NULL;

		// スクリーンの幅
		int width_ = 0;

		// スクリーンの高さ
		int height_ = 0;

		// ウィンドウモード
		bool bWindowed_ = false;

		// デバイス
		ID3D11Device* pD3DDevice_ = nullptr;

		// コンテキスト
		ID3D11DeviceContext* pImmediateContext_ = nullptr;

		// スワップチェイン
		IDXGISwapChain* pSwapChain_ = nullptr;

		// ドライバータイプ
		D3D_DRIVER_TYPE driverType_ = D3D_DRIVER_TYPE_UNKNOWN;

		// フューチャーレベル
		D3D_FEATURE_LEVEL featureLevel_ = D3D_FEATURE_LEVEL_11_0;

		// レンダーターゲットビュー
		ID3D11RenderTargetView* pRenderTargetViews_[RENDER_TARGET_VIEW_MAX] = {};

		// シェーダーリソースビュー
		ID3D11ShaderResourceView* pShaderResourceViews_[RENDER_TARGET_VIEW_MAX] = {};

		// デプスステンシルビュー
		ID3D11DepthStencilView* pDepthStencilView_ = nullptr;

		// シェーダーリソースビュー
		ID3D11ShaderResourceView* pDepthShaderResourceView_ = nullptr;

		// ラスタライザ―ステート
		ID3D11RasterizerState* pRsState_ = nullptr;

		// デプスステンシルステート
		ID3D11DepthStencilState* pDsState_ = nullptr;

		// ブレンドステート
		BLEND_STATE blendState_ = BLEND_STATE::BLEND_NONE;

		// サンプルディスク
		DXGI_SAMPLE_DESC MSAA_ = {};

		// ペラ用の頂点シェーダポインタ
		ID3D11VertexShader* pPeraVertexShader_ = nullptr;

		// 通常描画用のピクセルシェーダポインタ
		ID3D11PixelShader* pDefaultPixelShader_ = nullptr;

		// ペラ描画用のピクセルシェーダポインタ
		ID3D11PixelShader* pPeraPixelShader_ = nullptr;

		// 影描画ペラ用のピクセルシェーダポインタ
		ID3D11PixelShader* pPeraShadowPixelShader_ = nullptr;

		// ペラ用のレイアウト
		ID3D11InputLayout* pPeraVertexLayout_ = nullptr;

		// ペラ用の頂点バッファポインタ
		ID3D11Buffer* pPeraVertexBuffer_ = nullptr;

		// ステンシルの値
		UINT stencilRef_ = 0U;

		// レンダーターゲットの作成
		inline bool CreateRenderTarget();

		GraphicManager();
		~GraphicManager(){ UnInit(); }

		// コピーコンストラクタの禁止
		GraphicManager(const GraphicManager& src){}
		GraphicManager& operator=(const GraphicManager& src){}
	};
}

#endif // #ifndef INCLUDE_IDEA_GRAPHICMANAGER_H