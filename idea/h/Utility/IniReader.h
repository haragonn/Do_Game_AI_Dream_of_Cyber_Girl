/*==============================================================================
	[IniReader.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_INIREADER_H
#define INCLUDE_IDEA_INIREADER_H

namespace idea{
	//------------------------------------------------------------------------------
	// �N���X���@�FIniReader
	// �N���X�T�v�Fini�t�@�C���̓ǂݏ������s���N���X
	//------------------------------------------------------------------------------
	class IniReader{
	public:
		IniReader();											// �R���X�g���N�^
		IniReader(const char* pFileName, bool bCreate = true);	//
		bool LoadData(const char* pFileName, bool bCreate = true);	// ini�t�@�C�����J��
		int ReadInt(const char* pSection, const char* pKey, int def = 0);	// �����̓ǂݍ���
		int ReadString(const char* pSection, const char* pKey, char* pBuffer, int size, const char* def = "");	// ������̓ǂݍ���
		bool WriteInt(const char* pSection, const char* pKey, int value);			// �����̏�������
		bool WriteString(const char* pSection, const char* pKey, char* pString);	// ������̏�������

	private:
		char fullPath_[260] = {};	// �t�@�C���p�X
	};
}

#endif	// #ifndef INCLUDE_IDEA_INIREADER_H