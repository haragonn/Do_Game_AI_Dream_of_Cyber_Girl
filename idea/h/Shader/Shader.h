/*==============================================================================
	[Shader.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_SHARDER_H
#define INCLUDE_IDEA_SHARDER_H

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

struct ID3D11PixelShader;

namespace idea{
	// �V�F�[�_�̓ǂݍ���
	inline int ReadShader(const char* pFileName, BYTE** byteArray);

	//------------------------------------------------------------------------------
	// �N���X���@�FPixelShader
	// �N���X�T�v�F�s�N�Z���V�F�[�_�̓ǂݍ��݂ƊǗ����s���N���X
	//------------------------------------------------------------------------------
	class PixelShader{
	public:
		PixelShader();
		~PixelShader();

		// CSO�t�@�C������s�N�Z���V�F�[�_��ǂݍ���
		bool LoadPixelShaderFromCSO(const char* pFileName);

		// �A�[�J�C�u�t�@�C���̒���CSO�t�@�C������s�N�Z���V�F�[�_��ǂݍ���
		bool LoadPixelShaderFromArchiveFile(const char* pArchiveFileName, const char* pFileName);

		// �s�N�Z���V�F�[�_�|�C���^���擾
		ID3D11PixelShader* GetPixelShaderPtr()const
		{
			return pPixelShader_;
		}

	private:
		// �s�N�Z���V�F�[�_�|�C���^
		ID3D11PixelShader* pPixelShader_ = nullptr;
	};
}

#endif	// #ifndef INCLUDE_IDEA_SHARDER_H
