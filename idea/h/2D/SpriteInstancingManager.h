/*==============================================================================
	[SpriteInstancingManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_SPRITEINSTANCINGMANAGER_H
#define INCLUDE_IDEA_SPRITEINSTANCINGMANAGER_H

//------------------------------------------------------------------------------
// �O���錾
//------------------------------------------------------------------------------
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11ShaderResourceView;
struct ID3D11InputLayout;

namespace idea{
	//------------------------------------------------------------------------------
	// �N���X���@�FSpriteInstancingManager
	// �N���X�T�v�F�V���O���g��,SpriteInstancing�̕`��ɕK�v�ȃ��\�[�X���Ǘ�����N���X
	//------------------------------------------------------------------------------
	class SpriteInstancingManager{
	public:
		// �����ɕ`��ł���X�v���C�g�̍ő�l
		static const int INSTANCE_MAX = 65536;

		// �B��̃C���X�^���X��Ԃ�
		static SpriteInstancingManager& Instance()
		{
			static SpriteInstancingManager s_Instance;
			return s_Instance;
		}

		// ������
		bool Init();

		// �I������
		void UnInit();

		// ���_�V�F�[�_�̃|�C���^�̎擾
		inline ID3D11VertexShader* GetVertexShederPtr()const
		{
			return pVertexShader_;
		}

		// �C���X�^���V���O�o�b�t�@�̃|�C���^�̎擾
		inline ID3D11Buffer* GetInstanceDataBufferPtr()const
		{
			return pInstanceDataBuffer_;
		}

		// ���\�[�X�r���[�̃|�C���^�̎擾
		inline ID3D11ShaderResourceView* GetTransformShaderResourceViewPtr()const
		{
			return pTransformShaderResourceView_;
		}

	private:
		// ���_�V�F�[�_�̃|�C���^
		ID3D11VertexShader* pVertexShader_ = nullptr;

		// �C���X�^���V���O�o�b�t�@�̃|�C���^
		ID3D11Buffer* pInstanceDataBuffer_ = nullptr;

		// ���\�[�X�r���[�̃|�C���^
		ID3D11ShaderResourceView* pTransformShaderResourceView_ = nullptr;

		// �����̋֎~
		SpriteInstancingManager();
		~SpriteInstancingManager();

		// �R�s�[�R���X�g���N�^�̋֎~
		SpriteInstancingManager(const SpriteInstancingManager& src){}
		SpriteInstancingManager& operator=(const SpriteInstancingManager& src){}
	};

}
#endif	// #ifndef INCLUDE_IDEA_SPRITEINSTANCINGMANAGER_H