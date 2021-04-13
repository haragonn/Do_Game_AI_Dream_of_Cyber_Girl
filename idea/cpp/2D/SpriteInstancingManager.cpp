/*==============================================================================
	[SpriteInstancingManager.cpp]
														Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
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

// �V�F�[�_�[���A�[�J�C�u�t�@�C������ǂݍ��ނ��̐ݒ�
#define SHADER_FROM_ARCHIVE_FILE	(TRUE)

//------------------------------------------------------------------------------
// �萔�Q
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
// �R���X�g���N�^
//------------------------------------------------------------------------------
SpriteInstancingManager::SpriteInstancingManager() :
	pVertexShader_(nullptr),
	pInstanceDataBuffer_(nullptr),
	pTransformShaderResourceView_(nullptr)
{}

//------------------------------------------------------------------------------
// �f�X�g���N�^
//------------------------------------------------------------------------------
SpriteInstancingManager::~SpriteInstancingManager()
{
	UnInit();
}

//------------------------------------------------------------------------------
// ������
//------------------------------------------------------------------------------
bool SpriteInstancingManager::Init()
{
	// �������ł��Ă��Ȃ���ΏI��
	GraphicManager& gm = GraphicManager::Instance();
	if(!gm.GetContextPtr()){
		return false;
	}

	HRESULT hr;

#if SHADER_FROM_ARCHIVE_FILE
	{
		// ���_�V�F�[�_�쐬
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
		// ���_�V�F�[�_�쐬
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

	// �X�g���N�`���o�b�t�@
	{
		// �o�b�t�@�̐ݒ�
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

		// �V�F�[�_�[���\�[�X�̐ݒ�
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;   // �g�����ꂽ�o�b�t�@�[�ł��邱�Ƃ��w�肷��
		srvDesc.BufferEx.FirstElement = 0;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.BufferEx.NumElements = INSTANCE_MAX; // ���\�[�X���̗v�f�̐�

		// �\�����o�b�t�@�[�����ƂɃV�F�[�_�[���\�[�X�r���[���쐬����
		hr = gm.GetDevicePtr()->CreateShaderResourceView(pInstanceDataBuffer_, &srvDesc, &pTransformShaderResourceView_);
		if(FAILED(hr)){
			return false;
		}
	}

	return true;
}

//------------------------------------------------------------------------------
// �I������
//------------------------------------------------------------------------------
void SpriteInstancingManager::UnInit()
{
	// ���\�[�X�̃����[�X
	SafeRelease(pTransformShaderResourceView_);
	SafeRelease(pInstanceDataBuffer_);
	SafeRelease(pVertexShader_);
}
