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
	// �N���X���@�FTexture
	// �N���X�T�v�F�摜�̓ǂݍ���,�e�N�X�`�����\�[�X�̊Ǘ����s��
	//------------------------------------------------------------------------------
	class Texture{
	public:
		Texture();
		~Texture();

		// �摜�̓ǂݍ���
		bool LoadImageFromFile(const char* pFileName, unsigned int divX = 1U, unsigned int divY = 1U);
		
		// �A�[�J�C�u�t�@�C������摜��ǂݍ���
		bool LoadImageFromArchiveFile(const char* pArchiveFileName, const char* pFileName, unsigned int divX = 1U, unsigned int divY = 1U);

		// �X�g���[�W����摜��ǂݍ���
		bool LoadImageFromStorage(const char* pFileName, unsigned int divX = 1U, unsigned int divY = 1U);

		// �X�g���[�W����摜��ǂݍ���
		bool LoadImageFromTextureView(ID3D11ShaderResourceView* textureView, unsigned int divX = 1U, unsigned int divY = 1U);

		// �摜�̔j��
		void UnLoad();

		// �e�N�X�`�����\�[�X�r���[�̃|�C���^�\�̎擾
		ID3D11ShaderResourceView* GetTextureViewPtr()const{ return pTextureView_; }

		// �������̎擾(��)
		float GetDivU()const{ return divU_; }

		// �������̎擾(�c)
		float GetDivV()const{ return divV_; }

	private:
		friend class StorageManager;
		friend class TextureHolder;

		// �t�@�C����
		char fileName_[256] = {};

		// �e�N�X�`�����\�[�X�r���[�̃|�C���^
		ID3D11ShaderResourceView* pTextureView_ = nullptr;

		// ������(��)
		float divU_ = 0U;
		
		// ������(�c)
		float divV_ = 0U;

		// �z���_�[���X�g
		std::vector<TextureHolder*> vecTexHolderPtr_;

		// �X�g���[�W�g�p�t���O
		bool bStorage_ = false;

		// �R�s�[�R���X�g���N�^�̋֎~
		Texture(const Texture& src){}
		Texture& operator=(const Texture& src){}
	};
}
#endif	// #ifndef INCLUDE_IDEA_TEXTURE_H
