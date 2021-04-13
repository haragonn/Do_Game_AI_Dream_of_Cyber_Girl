/*==============================================================================
	[SpriteManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_SPRITEMANAGER_H
#define INCLUDE_IDEA_SPRITEMANAGER_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "../../../idea/h/Shader/Shader.h"

//------------------------------------------------------------------------------
// �O���錾
//------------------------------------------------------------------------------
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11ShaderResourceView;
struct ID3D11InputLayout;

namespace idea{
	//------------------------------------------------------------------------------
	// �N���X���@�FSpriteManager
	// �N���X�T�v�F�V���O���g��,Sprite�̕`��ɕK�v�ȃ��\�[�X���Ǘ�����N���X
	//------------------------------------------------------------------------------
	class SpriteManager{
	public:
		// ��`�̒��_��
		static const int RECT_VERTEX_NUM = 4;

		// �~�̒��_��
		static const int CIRCLE_VERTEX_NUM = 512;

		// �B��̃C���X�^���X��Ԃ�
		static SpriteManager& Instance()
		{
			static SpriteManager s_Instance;
			return s_Instance;
		}

		// ������
		bool Init();

		// �I������
		void UnInit();

		// ��`�̒��_�o�b�t�@�̃|�C���^�̎擾
		inline ID3D11Buffer* GetRectVertexBufferPtr()const
		{
			return pRectVertexBuffer_;
		}

		// �~�̒��_�o�b�t�@�̃|�C���^�̎擾
		inline ID3D11Buffer* GetCircleVertexBufferPtr()const
		{
			return pCircleVertexBuffer_;
		}

		// �~�̃C���f�b�N�X�o�b�t�@�̃|�C���^�̎擾
		inline ID3D11Buffer* GetCircleIndexBufferPtr()const
		{
			return pCircleIndexBuffer_;
		}

		// �C���v�b�g���C�A�E�g�̃|�C���^�̎擾
		inline ID3D11InputLayout* GetInputLayoutPtr()const
		{
			return pVertexLayout_;
		}

		// �萔�o�b�t�@�̃|�C���^�̎擾
		inline ID3D11Buffer* GetConstBufferPtr()const
		{
			return pConstBuffer_;
		}

		// ���_�V�F�[�_�̃|�C���^�̎擾
		inline ID3D11VertexShader* GetVertexShederPtr()const
		{
			return pVertexShader_;
		}

		// �ʏ�`��p�s�N�Z���V�F�[�_�̃|�C���^�̎擾
		inline ID3D11PixelShader* GetPixelShederDefaultPtr()const
		{
			return pixelShaderDefault_.GetPixelShaderPtr();
		}

		// �e�N�X�`���`��p�s�N�Z���V�F�[�_�̃|�C���^�̎擾
		inline ID3D11PixelShader* GetPixelShederTexturePtr()const
		{
			return pixelShaderTexture_.GetPixelShaderPtr();
		}

	private:
		// ��`�̒��_�o�b�t�@�̃|�C���^
		ID3D11Buffer* pRectVertexBuffer_ = nullptr;

		// �~�̒��_�o�b�t�@�̃|�C���^
		ID3D11Buffer* pCircleVertexBuffer_ = nullptr;

		// �~�̃C���f�b�N�X�o�b�t�@�̃|�C���^
		ID3D11Buffer* pCircleIndexBuffer_ = nullptr;

		// �C���v�b�g���C�A�E�g�̃|�C���^
		ID3D11InputLayout* pVertexLayout_ = nullptr;

		// �萔�o�b�t�@�̃|�C���^
		ID3D11Buffer* pConstBuffer_ = nullptr;

		// ���_�V�F�[�_�̃|�C���^
		ID3D11VertexShader* pVertexShader_ = nullptr;

		// �ʏ�`��p�s�N�Z���V�F�[�_�̃|�C���^
		PixelShader pixelShaderDefault_;

		// �e�N�X�`���`��p�s�N�Z���V�F�[�_�̃|�C���^
		PixelShader pixelShaderTexture_;

		// �����̋֎~
		SpriteManager();
		~SpriteManager();

		// �R�s�[�R���X�g���N�^�̋֎~
		SpriteManager(const SpriteManager& src){}
		SpriteManager& operator=(const SpriteManager& src){}
	};
}
#endif	// #ifndef INCLUDE_IDEA_SPRITEMANAGER_H