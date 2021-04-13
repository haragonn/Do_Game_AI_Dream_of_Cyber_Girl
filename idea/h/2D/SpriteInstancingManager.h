/*==============================================================================
	[SpriteInstancingManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_SPRITEINSTANCINGMANAGER_H
#define INCLUDE_IDEA_SPRITEINSTANCINGMANAGER_H

//------------------------------------------------------------------------------
// 前方宣言
//------------------------------------------------------------------------------
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11ShaderResourceView;
struct ID3D11InputLayout;

namespace idea{
	//------------------------------------------------------------------------------
	// クラス名　：SpriteInstancingManager
	// クラス概要：シングルトン,SpriteInstancingの描画に必要なリソースを管理するクラス
	//------------------------------------------------------------------------------
	class SpriteInstancingManager{
	public:
		// 同時に描画できるスプライトの最大値
		static const int INSTANCE_MAX = 65536;

		// 唯一のインスタンスを返す
		static SpriteInstancingManager& Instance()
		{
			static SpriteInstancingManager s_Instance;
			return s_Instance;
		}

		// 初期化
		bool Init();

		// 終了処理
		void UnInit();

		// 頂点シェーダのポインタの取得
		inline ID3D11VertexShader* GetVertexShederPtr()const
		{
			return pVertexShader_;
		}

		// インスタンシングバッファのポインタの取得
		inline ID3D11Buffer* GetInstanceDataBufferPtr()const
		{
			return pInstanceDataBuffer_;
		}

		// リソースビューのポインタの取得
		inline ID3D11ShaderResourceView* GetTransformShaderResourceViewPtr()const
		{
			return pTransformShaderResourceView_;
		}

	private:
		// 頂点シェーダのポインタ
		ID3D11VertexShader* pVertexShader_ = nullptr;

		// インスタンシングバッファのポインタ
		ID3D11Buffer* pInstanceDataBuffer_ = nullptr;

		// リソースビューのポインタ
		ID3D11ShaderResourceView* pTransformShaderResourceView_ = nullptr;

		// 生成の禁止
		SpriteInstancingManager();
		~SpriteInstancingManager();

		// コピーコンストラクタの禁止
		SpriteInstancingManager(const SpriteInstancingManager& src){}
		SpriteInstancingManager& operator=(const SpriteInstancingManager& src){}
	};

}
#endif	// #ifndef INCLUDE_IDEA_SPRITEINSTANCINGMANAGER_H