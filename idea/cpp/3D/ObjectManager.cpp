/*==============================================================================
	[ObjectManager.cpp]
														Author	:	Keigo Hara
==============================================================================*/
#include "../../h/3D/ObjectManager.h"
#include "../../h/3D/3DStruct.h"
#include "../../h/Utility/ConstBufferType.h"
#include "../../h/Framework/GraphicManager.h"
#include "../../h/Shader/Shader.h"
#include "../../h/Archive/ArchiveLoader.h"
#include "../../h/Utility/ideaMath.h"
#include "../../h/Utility/ideaUtility.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <directxmath.h>
#include <vector>

// シェーダーをアーカイブファイルから読み込むかの設定
#define SHADER_FROM_ARCHIVE_FILE	(TRUE)

namespace{
	static const char* ARCHIVE_FILENAME = "shader.dat";
	static const char* VS_FILENAME = "VS3D.cso";
	static const char* VS_BILLBOARD_FILENAME = "VS3DBillboard.cso";
	static const char* VS_WEIGHT_FILENAME = "VS3DWeight.cso";
	static const char* VS_SHADOW_FILENAME = "VS3DShadow.cso";
	static const char* PS_DEF_FILENAME = "PSDefault.cso";
	static const char* PS_DEF_SHADOW_FILENAME = "PSShadow.cso";
	static const char* PS_TEX_FILENAME = "PSTexture.cso";
	static const char* PS_TEX_SHADOW_FILENAME = "PSTextureShadow.cso";
}

using namespace DirectX;
using namespace std;
using namespace idea;

Vector3D ObjectManager::s_Light_;

ObjectManager::ObjectManager() :
	pVertexShader_(nullptr),
	pBillboardVertexShader_(nullptr),
	pWeightVertexShader_(nullptr),
	pShadowVertexShader_(nullptr),

	pVertexLayout_(nullptr),
	pBillboardVertexLayout_(nullptr),
	pShadowVertexLayout_(nullptr),
	pWeightVertexLayout_(nullptr),

	pConstBuffer_(nullptr),
	pWeightConstBuffer_(nullptr),
	pShadowConstBuffer_(nullptr),
	pMaterialConstBuffer_(nullptr),
	pWorldConstBuffer_(nullptr),
	pVPConstBuffer_(nullptr),
	pLightConstBuffer_(nullptr),
	pCameraConstBuffer_(nullptr),

	pCubeVertexBuffer_(nullptr),
	pCubeIndexBuffer_(nullptr),
	cubeIndexSize_(0U),

	pSphereVertexBuffer_(nullptr),
	pSphereIndexBuffer_(nullptr),
	sphereIndexSize_(0U),

	pBillboardVertexBuffer_(nullptr),
	pBillboardIndexBuffer_(nullptr)
{
	s_Light_ = Vector3D(0.25f, -1.0f, 0.5f);
}

bool ObjectManager::Init()
{
	GraphicManager& gm = GraphicManager::Instance();
	if(!gm.GetContextPtr()){
		return false;
	}

	HRESULT hr;


#if SHADER_FROM_ARCHIVE_FILE
	// デフォルト頂点シェーダ
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

		// 入力レイアウト定義
		D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		UINT elem_num = ARRAYSIZE(layout);

		// 入力レイアウト作成
		hr = gm.GetDevicePtr()->CreateInputLayout(layout, elem_num, &loader.GetData()[0], loader.GetSize(), &pVertexLayout_);
		if(FAILED(hr)){
			return false;
		}
	}

	// ビルボード用頂点シェーダ
	{
		// 頂点シェーダ作成
		ArchiveLoader loader;
		if(!loader.Load(ARCHIVE_FILENAME, VS_BILLBOARD_FILENAME)){
			return false;
		}
		hr = gm.GetDevicePtr()->CreateVertexShader(&loader.GetData()[0], loader.GetSize(), NULL, &pBillboardVertexShader_);
		if(FAILED(hr)){
			return false;
		}

		// 入力レイアウト定義
		D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		UINT elem_num = ARRAYSIZE(layout);

		// 入力レイアウト作成
		hr = gm.GetDevicePtr()->CreateInputLayout(layout, elem_num, &loader.GetData()[0], loader.GetSize(), &pBillboardVertexLayout_);
		if(FAILED(hr)){
			return false;
		}
	}

	// 重み付き頂点シェーダ
	{
		// 頂点シェーダ作成
		ArchiveLoader loader;
		if(!loader.Load(ARCHIVE_FILENAME, VS_WEIGHT_FILENAME)){
			return false;
		}
		hr = gm.GetDevicePtr()->CreateVertexShader(&loader.GetData()[0], loader.GetSize(), NULL, &pWeightVertexShader_);
		if(FAILED(hr)){
			return false;
		}

		// 入力レイアウト定義
		D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		UINT elem_num = ARRAYSIZE(layout);

		// 入力レイアウト作成
		hr = gm.GetDevicePtr()->CreateInputLayout(layout, elem_num, &loader.GetData()[0], loader.GetSize(), &pWeightVertexLayout_);
		if(FAILED(hr)){
			return false;
		}
	}

	// 影描画頂点シェーダ
	{
		// 頂点シェーダ作成
		ArchiveLoader loader;
		if(!loader.Load(ARCHIVE_FILENAME, VS_SHADOW_FILENAME)){
			return false;
		}
		hr = gm.GetDevicePtr()->CreateVertexShader(&loader.GetData()[0], loader.GetSize(), NULL, &pShadowVertexShader_);
		if(FAILED(hr)){
			return false;
		}

		// 入力レイアウト定義
		D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		UINT elem_num = ARRAYSIZE(layout);

		// 入力レイアウト作成
		hr = gm.GetDevicePtr()->CreateInputLayout(layout, elem_num, &loader.GetData()[0], loader.GetSize(), &pShadowVertexLayout_);
		if(FAILED(hr)){
			return false;
		}
	}
#else
	// デフォルト頂点シェーダ
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

		// 入力レイアウト定義
		D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		UINT elem_num = ARRAYSIZE(layout);

		// 入力レイアウト作成
		hr = gm.GetDevicePtr()->CreateInputLayout(layout, elem_num, data, size, &pVertexLayout_);
		if(FAILED(hr)){
			SafeDeleteArray(data);
			return false;
		}

		SafeDeleteArray(data);
	}

	// ビルボード用頂点シェーダ
	{
		// 頂点シェーダ作成
		BYTE* data;
		int size = 0;
		size = ReadShader(VS_BILLBOARD_FILENAME, &data);
		if(size == 0){
			return false;
		}

		hr = gm.GetDevicePtr()->CreateVertexShader(data, size, NULL, &pBillboardVertexShader_);
		if(FAILED(hr)){
			SafeDeleteArray(data);
			return false;
		}

		// 入力レイアウト定義
		D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		UINT elem_num = ARRAYSIZE(layout);

		// 入力レイアウト作成
		hr = gm.GetDevicePtr()->CreateInputLayout(layout, elem_num, data, size, &pBillboardVertexLayout_);
		if(FAILED(hr)){
			SafeDeleteArray(data);
			return false;
		}

		SafeDeleteArray(data);
	}

	// 重み付き頂点シェーダ
	{
		// 頂点シェーダ作成
		BYTE* data;
		int size = 0;
		size = ReadShader(VS_WEIGHT_FILENAME, &data);
		if(size == 0){
			return false;
		}

		hr = gm.GetDevicePtr()->CreateVertexShader(data, size, NULL, &pWeightVertexShader_);
		if(FAILED(hr)){
			SafeDeleteArray(data);
			return false;
		}

		// 入力レイアウト定義
		D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		UINT elem_num = ARRAYSIZE(layout);

		// 入力レイアウト作成
		hr = gm.GetDevicePtr()->CreateInputLayout(layout, elem_num, data, size, &pWeightVertexLayout_);
		if(FAILED(hr)){
			SafeDeleteArray(data);
			return false;
		}

		SafeDeleteArray(data);
	}

	// 影描画頂点シェーダ
	{
		// 頂点シェーダ作成
		BYTE* data;
		int size = 0;
		size = ReadShader(VS_SHADOW_FILENAME, &data);
		if(size == 0){
			return false;
		}

		hr = gm.GetDevicePtr()->CreateVertexShader(data, size, NULL, &pShadowVertexShader_);
		if(FAILED(hr)){
			SafeDeleteArray(data);
			return false;
		}

		// 入力レイアウト定義
		D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		UINT elem_num = ARRAYSIZE(layout);

		// 入力レイアウト作成
		hr = gm.GetDevicePtr()->CreateInputLayout(layout, elem_num, data, size, &pShadowVertexLayout_);
		if(FAILED(hr)){
			SafeDeleteArray(data);
			return false;
		}

		SafeDeleteArray(data);
	}

#endif

	// ピクセルシェーダ
#if SHADER_FROM_ARCHIVE_FILE
	// ピクセルシェーダの読み込み(通常)
	if(!pixelShaderDefault_.LoadPixelShaderFromArchiveFile(ARCHIVE_FILENAME, PS_DEF_FILENAME)){
		return false;
	}
	// ピクセルシェーダの読み込み(影)
	if(!pixelShaderShadow_.LoadPixelShaderFromArchiveFile(ARCHIVE_FILENAME, PS_DEF_SHADOW_FILENAME)){
		return false;
	}
	// ピクセルシェーダの読み込み(テクスチャ)
	if(!pixelShaderTexture_.LoadPixelShaderFromArchiveFile(ARCHIVE_FILENAME, PS_TEX_FILENAME)){
		return false;
	}
	// ピクセルシェーダの読み込み(テクスチャ＋影)
	if(!pixelShaderTextureShadow_.LoadPixelShaderFromArchiveFile(ARCHIVE_FILENAME, PS_TEX_SHADOW_FILENAME)){
		return false;
	}
#else
	// ピクセルシェーダの読み込み(通常)
	if(!pixelShaderDefault_.LoadPixelShaderFromCSO(PS_DEF_FILENAME)){
		return false;
	}
	// ピクセルシェーダの読み込み(影)
	if(!pixelShaderShadow_.LoadPixelShaderFromCSO(PS_DEF_SHADOW_FILENAME)){
		return false;
	}
	// ピクセルシェーダの読み込み(テクスチャ)
	if(!pixelShaderTexture_.LoadPixelShaderFromCSO(PS_TEX_FILENAME)){
		return false;
	}
	// ピクセルシェーダの読み込み(テクスチャ＋影)
	if(!pixelShaderTextureShadow_.LoadPixelShaderFromCSO(PS_TEX_SHADOW_FILENAME)){
		return false;
	}
#endif

	// 定数バッファ
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(ConstBuffer3D);
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		hr = gm.GetDevicePtr()->CreateBuffer(&bd, nullptr, &pConstBuffer_);
		if(FAILED(hr)){
			return false;
		}
	}

	// ワールド座標情報
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(WorldConstBuffer);
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		hr = gm.GetDevicePtr()->CreateBuffer(&bd, nullptr, &pWorldConstBuffer_);
		if(FAILED(hr)){
			return false;
		}
	}

	// VP情報
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(VPConstBuffer);
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		hr = gm.GetDevicePtr()->CreateBuffer(&bd, nullptr, &pVPConstBuffer_);
		if(FAILED(hr)){
			return false;
		}
	}

	// ライト情報
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(LightConstBuffer);
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		hr = gm.GetDevicePtr()->CreateBuffer(&bd, nullptr, &pLightConstBuffer_);
		if(FAILED(hr)){
			return false;
		}
	}

	// カメラ情報
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(CameraConstBuffer);
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		hr = gm.GetDevicePtr()->CreateBuffer(&bd, nullptr, &pCameraConstBuffer_);
		if(FAILED(hr)){
			return false;
		}
	}

	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(MaterialConstBuffer);
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		hr = gm.GetDevicePtr()->CreateBuffer(&bd, nullptr, &pMaterialConstBuffer_);
		if(FAILED(hr)){
			return false;
		}
	}

	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(BoneWorldConstBuffer);
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		hr = gm.GetDevicePtr()->CreateBuffer(&bd, nullptr, &pWeightConstBuffer_);
		if(FAILED(hr)){
			return false;
		}
	}

	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(ShadowConstBuffer);
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		hr = gm.GetDevicePtr()->CreateBuffer(&bd, nullptr, &pShadowConstBuffer_);
		if(FAILED(hr)){
			return false;
		}
	}

	// 箱
	// 頂点バッファ
	{
		MeshVertexData cubeVertexList[] = {
		{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, ideaColor::WHITE, { 0.0f, 0.0f } },
		{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, ideaColor::WHITE, { 1.0f, 0.0f } },
		{ { -0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, ideaColor::WHITE, { 0.0f, 1.0f } },
		{ {  0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, ideaColor::WHITE, { 1.0f, 1.0f } },

		{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, ideaColor::WHITE, { 1.0f, 0.0f } },
		{ { -0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, ideaColor::WHITE, { 1.0f, 1.0f } },
		{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, ideaColor::WHITE, { 0.0f, 0.0f } },
		{ {  0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, ideaColor::WHITE, { 0.0f, 1.0f } },

		{ { -0.5f,  0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, ideaColor::WHITE, { 0.0f, 0.0f } },
		{ { -0.5f,  0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, ideaColor::WHITE, { 1.0f, 0.0f } },
		{ { -0.5f, -0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, ideaColor::WHITE, { 0.0f, 1.0f } },
		{ { -0.5f, -0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, ideaColor::WHITE, { 1.0f, 1.0f } },

		{ {  0.5f,  0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, ideaColor::WHITE, { 1.0f, 0.0f } },
		{ {  0.5f, -0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, ideaColor::WHITE, { 1.0f, 1.0f } },
		{ {  0.5f,  0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, ideaColor::WHITE, { 0.0f, 0.0f } },
		{ {  0.5f, -0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, ideaColor::WHITE, { 0.0f, 1.0f } },

		{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, ideaColor::WHITE, { 0.0f, 0.0f } },
		{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, ideaColor::WHITE, { 1.0f, 0.0f } },
		{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, ideaColor::WHITE, { 0.0f, 1.0f } },
		{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, ideaColor::WHITE, { 1.0f, 1.0f } },

		{ { -0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, ideaColor::WHITE, { 1.0f, 0.0f } },
		{ { -0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, ideaColor::WHITE, { 1.0f, 1.0f } },
		{ {  0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, ideaColor::WHITE, { 0.0f, 0.0f } },
		{ {  0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, ideaColor::WHITE, { 0.0f, 1.0f } },
		};

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(MeshVertexData) * ArraySize(cubeVertexList);
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = cubeVertexList;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		hr = gm.GetDevicePtr()->CreateBuffer(&bd, &InitData, &pCubeVertexBuffer_);
		if(FAILED(hr)){
			return false;
		}
	}

	// インデックスバッファ
	{
		WORD cubeIndexList[] = {
			 0,  1,  2,   3,  2,  1,
			 4,  5,  6,   7,  6,  5,
			 8,  9, 10,  11, 10,  9,
			12, 13, 14,  15, 14, 13,
			16, 17, 18,  19, 18, 17,
			20, 21, 22,  23, 22, 21,
		};

		cubeIndexSize_ = ArraySize(cubeIndexList);

		D3D11_BUFFER_DESC bd;
		bd.ByteWidth = sizeof(WORD) * cubeIndexSize_;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = cubeIndexList;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		hr = gm.GetDevicePtr()->CreateBuffer(&bd, &InitData, &pCubeIndexBuffer_);
		if(FAILED(hr)){
			return false;
		}
	}

	// 球
	int verticalSize = 16;
	int horizontalSize = 16 * 2;
	// 頂点バッファ
	{
		vector<MeshVertexData> vecVertex;
		float radius = 0.5f;

		for(int i = 0; i <= verticalSize; ++i){
			float v = 1.0f - (float)i / verticalSize * 30.0f;

			float latitude = (i * ideaMath::PI / verticalSize) - ideaMath::PIDIV2;
			float dy, dxz;

			XMScalarSinCos(&dy, &dxz, latitude);

			for(int j = 0; j <= horizontalSize; ++j){
				float u = 1.0f - (float)j / horizontalSize * 30.0f;

				float longitude = j * ideaMath::PI2 / horizontalSize;
				float dx, dz;

				XMScalarSinCos(&dx, &dz, longitude);

				dx *= dxz;
				dz *= dxz;

				Vector3D normal{ dx, dy, dz };

				MeshVertexData vertex;
				vertex.pos = normal * radius;
				vertex.nor = normal;
				vertex.color = ideaColor::WHITE;
				vertex.tex = Vector2D(u, v);


				vecVertex.push_back(vertex);
			}
		}

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(MeshVertexData) * vecVertex.size();
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = &vecVertex[0];
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		hr = gm.GetDevicePtr()->CreateBuffer(&bd, &InitData, &pSphereVertexBuffer_);
		if(FAILED(hr)){
			return false;
		}
	}

	// インデックスバッファ
	{
		vector<WORD> vecIndex;

		int stride = horizontalSize + 1;

		for(int i = 0; i <= verticalSize; ++i){
			for(int j = 0; j <= horizontalSize; ++j){
				int nextI = i + 1;
				int nextJ = (j + 1) % stride;

				vecIndex.push_back((WORD)(i * stride + nextJ));
				vecIndex.push_back((WORD)(nextI * stride + j));
				vecIndex.push_back((WORD)(i * stride + j));

				vecIndex.push_back((WORD)(nextI * stride + nextJ));
				vecIndex.push_back((WORD)(nextI * stride + j));
				vecIndex.push_back((WORD)(i * stride + nextJ));
			}
		}

		sphereIndexSize_ = vecIndex.size();

		D3D11_BUFFER_DESC bd;
		bd.ByteWidth = sizeof(WORD) * sphereIndexSize_;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = &vecIndex[0];
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		hr = gm.GetDevicePtr()->CreateBuffer(&bd, &InitData, &pSphereIndexBuffer_);
		if(FAILED(hr)){
			return false;
		}
	}


	// ビルボード
	// 頂点バッファ
	{
		BillboardVertexData billboardVertexList[] = {
		{ { -0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f } },
		{ {  0.5f,  0.5f, 0.0f }, { 1.0f, 0.0f } },
		{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f } },
		{ {  0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f } }
		};

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(BillboardVertexData) * 4;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = billboardVertexList;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		hr = gm.GetDevicePtr()->CreateBuffer(&bd, &InitData, &pBillboardVertexBuffer_);
		if(FAILED(hr)){
			return false;
		}
	}

	return true;
}

void ObjectManager::UnInit()
{
	SafeRelease(pBillboardIndexBuffer_);
	SafeRelease(pBillboardVertexBuffer_);

	SafeRelease(pSphereIndexBuffer_);
	SafeRelease(pSphereVertexBuffer_);

	SafeRelease(pCubeIndexBuffer_);
	SafeRelease(pCubeVertexBuffer_);

	SafeRelease(pConstBuffer_);
	SafeRelease(pWorldConstBuffer_);
	SafeRelease(pVPConstBuffer_);
	SafeRelease(pLightConstBuffer_);
	SafeRelease(pCameraConstBuffer_);
	SafeRelease(pMaterialConstBuffer_);
	SafeRelease(pWeightConstBuffer_);
	SafeRelease(pShadowConstBuffer_);

	SafeRelease(pVertexLayout_);
	SafeRelease(pBillboardVertexLayout_);
	SafeRelease(pWeightVertexLayout_);
	SafeRelease(pShadowVertexLayout_);

	SafeRelease(pVertexShader_);
	SafeRelease(pBillboardVertexShader_);
	SafeRelease(pWeightVertexShader_);
	SafeRelease(pShadowVertexShader_);
}

void ObjectManager::SetLight(float axisX, float axisY, float axisZ)
{
	s_Light_ = Vector3D(axisX, axisY, axisZ);
}
