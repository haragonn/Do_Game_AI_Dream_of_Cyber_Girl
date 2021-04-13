/*==============================================================================
	[Memory.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_MEMORY_H
#define INCLUDE_IDEA_MEMORY_H

namespace idea{
	//------------------------------------------------------------------------------
	// �N���X���@�FMemory
	// �N���X�T�v�F�V�[�����܂����Œl��ێ�����N���X
	//------------------------------------------------------------------------------
	class Memory{
	public:
		// �L�[�ɐ�����ݒ�
		void SetInt(const char* pKey, int value);

		// �����̎擾
		int GetInt(const char* pKey, int def = 0);

		// �w�肵���L�[�Ɗ֘A�t����ꂽ���������݂��邩
		bool HasKeyInt(const char* pKey);

		// �L�[�ɕ��������_����ݒ�
		void SetFloat(const char* pKey, float value);

		// ���������_���̎擾
		float GetFloat(const char* pKey, float def = 0.0f);

		// �w�肵���L�[�Ɗ֘A�t����ꂽ���������_�������݂��邩
		bool HasKeyFloat(const char* pKey);

		// �L�[�ɔ{���x���������_����ݒ�
		void SetDouble(const char* pKey, double value);

		// �{���x���������_���̎擾
		double GetDouble(const char* pKey, double def = 0.0);

		// �w�肵���L�[�Ɗ֘A�t����ꂽ�{���x���������_�������݂��邩
		bool HasKeyDouble(const char* pKey);
	};
}

#endif	// #ifndef INCLUDE_IDEA_MEMORY_H