/*==============================================================================
	[SpriteInstancingManager.cpp]
														Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../../h/2D/SpriteInstancingManager.h"
#include "../../h/Framework/GraphicManager.h"
#include "../../h/Archive/ArchiveLoader.h"
#include "../../h/Shader/Shader.h"
#include "../../h/Utility/ConstBufferType.h"
#include "../../h/Utility/ideaUtility.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <directxmath.h>

// シェーダーをアーカイブファイルから読み込むかの設定
#define SHADER_FROM_ARCHIVE_FILE	(TRUE)

//------------------------------------------------------------------------------
// 定数群
//------------------------------------------------------------------------------
namespace{
	static const char* ARCHIVE_FILENAME = "shader.dat";
	static const char* VS_FILENAME = "VS2DI.cso";
}

//------------------------------------------------------------------------------
// using namespace
//------------------------------------------------------------------------------
using namespace idea;
using namespace DirectX;

//------------------------------------------------------------------------------
// コンストラクタ
//------------------------------------------------------------------------------
SpriteInstancingManager::SpriteInstancingManager() :
	pVertexShader_(nullptr),
	pInstanceDataBuffer_(nullptr),
	pTransformShaderResourceView_(nullptr)
{}

//------------------------------------------------------------------------------
// デストラクタ
//------------------------------------------------------------------------------
SpriteInstancingManager::~SpriteInstancingManager()
{
	UnInit();
}

//------------------------------------------------------------------------------
// 初期化
//------------------------------------------------------------------------------
bool SpriteInstancingManager::Init()
{
	// 準備ができていなければ終了
	GraphicManager& gm = GraphicManager::Instance();
	if(!gm.GetContextPtr()){
		return false;
	}

	HRESULT hr;

#if SHADER_FROM_ARCHIVE_FILE
	{
		// 頂点シェーダ作成
		ArchiveLoader loader;
		if(!loader.Load(ARCHIVE_FILENAME, VS_FILENAME)){
			return false;
		}

		hr = gm.GetDevicePtr()->CreateVertexShader(&loader.GetData()[0], loader.GetSize(), NULL, &pVertexShader_);
		if(FAILED(hr)){
			return false;
		}
	}
#else
	{
		// 頂点シェーダ作成
		BYTE* data;
		int size = 0;
		size = ReadShader(VS_FILENAME, &data);
		if(size == 0){
			return false;
		}

		hr = gm.GetDevicePtr()->CreateVertexShader(data, size, NULL, &pVertexShader_);
		if(FAILED(hr)){
			SafeDeleteArray(data);
			return false;
		}

		SafeDeleteArray(data);
	}
#endif

	// ストラクチャバッファ
	{
		// バッファの設定
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(PerInstanceData) * INSTANCE_MAX;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bd.StructureByteStride = sizeof(PerInstanceData);

		hr = gm.GetDevicePtr()->CreateBuffer(&bd, NULL, &pInstanceDataBuffer_);
		if(FAILED(hr)){
			return false;
		}

		// シェーダーリソースの設定
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;   // 拡張されたバッファーであることを指定する
		srvDesc.BufferEx.FirstElement = 0;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.BufferEx.NumElements = INSTANCE_MAX; // リソース内の要素の数

		// 構造化バッファーをもとにシェーダーリソースビューを作成する
		hr = gm.GetDevicePtr()->CreateShaderResourceView(pInstanceDataBuffer_, &srvDesc, &pTransformShaderResourceView_);
		if(FAILED(hr)){
			return false;
		}
	}

	return true;
}

//------------------------------------------------------------------------------
// 終了処理
//------------------------------------------------------------------------------
void SpriteInstancingManager::UnInit()
{
	// リソースのリリース
	SafeRelease(pTransformShaderResourceView_);
	SafeRelease(pInstanceDataBuffer_);
	SafeRelease(pVertexShader_);
}
