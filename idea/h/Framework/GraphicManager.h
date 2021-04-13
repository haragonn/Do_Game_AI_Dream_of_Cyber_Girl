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
	// �u�����h�X�e�[�g
	enum class BLEND_STATE{
		// ��`�Ȃ�
		BLEND_NONE = 0,

		// �A���t�@�u�����h�Ȃ�
		BLEND_DEFAULT,

		// �A���t�@�u�����h����
		BLEND_ALIGNMENT,

		// ���Z����
		BLEND_ADD,

		// ���Z����
		BLEND_SUBTRACT,

		// ��Z����
		BLEND_MULTIPLE,

		// �ő�l
		BLEND_MAX
	};

	struct PeraVertex{
		// ���W
		Vector3D pos;

		// �e�N�X�`�����W
		Vector2D tex;
	};

	//------------------------------------------------------------------------------
	// �N���X���@�FGraphicManager
	// �N���X�T�v�F�V���O���g��
	// �@�@�@�@�@�@�f�o�C�X,�R���e�L�X�g,�f�v�X�o�b�t�@,���X�^���C�U�̊Ǘ����s��
	//------------------------------------------------------------------------------
	class GraphicManager{
	public:
		// �����_�[�^�[�Q�b�g�r���[�̐�
		static const int RENDER_TARGET_VIEW_MAX = 4;

		// �B��̃C���X�^���X��Ԃ�
		static GraphicManager& Instance()
		{
			static GraphicManager s_Instance;
			return s_Instance;
		}

		// ������
		bool Init(HWND hWnd, UINT width, UINT height, bool bWindowed, UINT fps = 60U, bool bMSAA = false);

		// �I������
		void UnInit();

		// �`��J�n
		bool BeginScene();

		// �`��I��
		bool EndScene();

		// �p�X�̕`��
		bool DrawPath(int target, int src, ID3D11PixelShader* pps, D3D11_VIEWPORT viewPort);

		// �f�v�X�o�b�t�@�̕`��
		bool DrawShadow(int target, D3D11_VIEWPORT viewPort);

		// �E�B���h�E�n���h���̎擾
		HWND GetHWND()const{ return hWnd_; }

		// �X�N���[�����̎擾
		int	GetWidth()const{ return width_; }

		// �X�N���[�������̎擾
		int GetHeight()const{ return height_; }

		// Direct3D�f�o�C�X�̎擾
		ID3D11Device* GetDevicePtr()const{ return pD3DDevice_; }

		// �R���e�L�X�g�̎擾
		ID3D11DeviceContext* GetContextPtr()const{ return pImmediateContext_; }

		// �f�v�X�X�e���V���r���[�̎擾
		ID3D11DepthStencilView* GetDepthStencilViewPtr(){ return pDepthStencilView_; }

		// �f�v�X�X�e���V���r���[�̎擾
		ID3D11ShaderResourceView* GetDepthShaderResourceViewPtr(){ return pDepthShaderResourceView_; }

		// �e�`��y���p�̃V�F�[�_�|�C���^�̎擾
		ID3D11PixelShader* GetPeraShadowPixelShaderPtr(){
			return pPeraShadowPixelShader_;
		}

		// �����_�[�^�[�Q�b�g�r���[�|�C���^�̎擾
		ID3D11RenderTargetView* GetRenderTargetViewPtr(int idx)
		{
			Clamp(idx, 0, RENDER_TARGET_VIEW_MAX - 1);
			return pRenderTargetViews_[idx];
		}

		// �����_�[�^�[�Q�b�g�r���[�z��|�C���^�̎擾
		ID3D11ShaderResourceView** GetShaderResourceViewPtrArray()
		{
			return pShaderResourceViews_;
		}

		// �y���|���S���̒��_�o�b�t�@�|�C���^�̎擾
		ID3D11Buffer* GetPeraVertexBufferPtr()const{ return pPeraVertexBuffer_; }

		// �W�����X�^���C�U�\�X�e�[�g�̎擾
		ID3D11RasterizerState* GetDefaultRasterizerStatePtr(){ return pRsState_; }

		// �W���[�x�X�e�[�g�̎擾
		ID3D11DepthStencilState* GetDefaultDepthStatePtr(){ return pDsState_; }

		// �u�����h�X�e�[�g��ݒ肷��
		bool SetBlendState(BLEND_STATE blendState);

		// �}�X�N�̕`��
		bool DrawMask(bool bVisible);

		// �}�X�N��AND����
		bool DrawAnd();

		// �}�X�N��XOR����
		bool DrawXor();

		// �}�X�N�̏I��
		bool EndMask();

		// 3D�`��̏���
		bool Draw3D();

		// �E�B���h�E���[�h�̐؂�ւ�
		bool ChangeDisplayMode(bool bWindowed);

	private:
		// �E�B���h�E�n���h��
		HWND hWnd_ = NULL;

		// �X�N���[���̕�
		int width_ = 0;

		// �X�N���[���̍���
		int height_ = 0;

		// �E�B���h�E���[�h
		bool bWindowed_ = false;

		// �f�o�C�X
		ID3D11Device* pD3DDevice_ = nullptr;

		// �R���e�L�X�g
		ID3D11DeviceContext* pImmediateContext_ = nullptr;

		// �X���b�v�`�F�C��
		IDXGISwapChain* pSwapChain_ = nullptr;

		// �h���C�o�[�^�C�v
		D3D_DRIVER_TYPE driverType_ = D3D_DRIVER_TYPE_UNKNOWN;

		// �t���[�`���[���x��
		D3D_FEATURE_LEVEL featureLevel_ = D3D_FEATURE_LEVEL_11_0;

		// �����_�[�^�[�Q�b�g�r���[
		ID3D11RenderTargetView* pRenderTargetViews_[RENDER_TARGET_VIEW_MAX] = {};

		// �V�F�[�_�[���\�[�X�r���[
		ID3D11ShaderResourceView* pShaderResourceViews_[RENDER_TARGET_VIEW_MAX] = {};

		// �f�v�X�X�e���V���r���[
		ID3D11DepthStencilView* pDepthStencilView_ = nullptr;

		// �V�F�[�_�[���\�[�X�r���[
		ID3D11ShaderResourceView* pDepthShaderResourceView_ = nullptr;

		// ���X�^���C�U�\�X�e�[�g
		ID3D11RasterizerState* pRsState_ = nullptr;

		// �f�v�X�X�e���V���X�e�[�g
		ID3D11DepthStencilState* pDsState_ = nullptr;

		// �u�����h�X�e�[�g
		BLEND_STATE blendState_ = BLEND_STATE::BLEND_NONE;

		// �T���v���f�B�X�N
		DXGI_SAMPLE_DESC MSAA_ = {};

		// �y���p�̒��_�V�F�[�_�|�C���^
		ID3D11VertexShader* pPeraVertexShader_ = nullptr;

		// �ʏ�`��p�̃s�N�Z���V�F�[�_�|�C���^
		ID3D11PixelShader* pDefaultPixelShader_ = nullptr;

		// �y���`��p�̃s�N�Z���V�F�[�_�|�C���^
		ID3D11PixelShader* pPeraPixelShader_ = nullptr;

		// �e�`��y���p�̃s�N�Z���V�F�[�_�|�C���^
		ID3D11PixelShader* pPeraShadowPixelShader_ = nullptr;

		// �y���p�̃��C�A�E�g
		ID3D11InputLayout* pPeraVertexLayout_ = nullptr;

		// �y���p�̒��_�o�b�t�@�|�C���^
		ID3D11Buffer* pPeraVertexBuffer_ = nullptr;

		// �X�e���V���̒l
		UINT stencilRef_ = 0U;

		// �����_�[�^�[�Q�b�g�̍쐬
		inline bool CreateRenderTarget();

		GraphicManager();
		~GraphicManager(){ UnInit(); }

		// �R�s�[�R���X�g���N�^�̋֎~
		GraphicManager(const GraphicManager& src){}
		GraphicManager& operator=(const GraphicManager& src){}
	};
}

#endif // #ifndef INCLUDE_IDEA_GRAPHICMANAGER_H